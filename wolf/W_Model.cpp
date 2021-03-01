//-----------------------------------------------------------------------------
// File:			W_Model.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------
#include "W_Model.h"
#include "W_Common.h"
#include "W_BufferManager.h"
#include "W_MaterialManager.h"


namespace wolf
{
static ComponentType gs_aPODTypeMap[] = 
{
    wolf::CT_Invalid,   //EPODDataNone,
	wolf::CT_Float,		//EPODDataFloat,
	wolf::CT_Int,		//EPODDataInt,
	wolf::CT_UShort,	//EPODDataUnsignedShort,
	wolf::CT_UByte4,	//EPODDataRGBA,
	wolf::CT_UByte4,	//EPODDataARGB,
	wolf::CT_UByte4,	//EPODDataD3DCOLOR,
	wolf::CT_UByte4,	//EPODDataUBYTE4,
	wolf::CT_Invalid,	//EPODDataDEC3N,
	wolf::CT_Invalid,	//EPODDataFixed16_16,
	wolf::CT_UByte,		//EPODDataUnsignedByte,
	wolf::CT_Short,		//EPODDataShort,
	wolf::CT_ShortNorm,	//EPODDataShortNorm,
	wolf::CT_Byte,		//EPODDataByte,
	wolf::CT_ByteNorm,	//EPODDataByteNorm,
	wolf::CT_UByteNorm, //EPODDataUnsignedByteNorm,
	wolf::CT_UShortNorm, //EPODDataUnsignedShortNorm,
	wolf::CT_UInt,		//EPODDataUnsignedInt
};

//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
Model::Model(const std::string& path, const MaterialProvider& matProvider) 
  : m_path(path)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(path.c_str(), 
											  aiProcess_Triangulate | 
                                              aiProcess_OptimizeMeshes | 
                                              aiProcess_TransformUVCoords | 
                                              aiProcess_CalcTangentSpace |
                                              aiProcess_JoinIdenticalVertices);

	// Build all the meshes
	for(unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh* pMesh = pScene->mMeshes[i];

		Model::InterleavedMesh* pInterleavedMesh = _interleaveMeshData(pMesh);

		// Create the vertex declaration
		wolf::VertexDeclaration* pDecl = new wolf::VertexDeclaration;
		pDecl->Begin();

        // Create the vertex buffer
		wolf::VertexBuffer* pVB = wolf::BufferManager::CreateVertexBuffer(pInterleavedMesh->pInterleavedData, pInterleavedMesh->interleavedDataSize);
        
		// Create the index buffer
		wolf::IndexBuffer* pIB = wolf::BufferManager::CreateIndexBuffer(pInterleavedMesh->pIndexData, pInterleavedMesh->numIndices);

		// We'll always have a position
		pDecl->AppendAttribute(wolf::AT_Position, 3, wolf::CT_Float, pInterleavedMesh->posOffset);

		if( pInterleavedMesh->normalOffset >= 0 )
			pDecl->AppendAttribute(wolf::AT_Normal, 3, wolf::CT_Float, pInterleavedMesh->normalOffset);

		if( pInterleavedMesh->tangentOffset >= 0 )
			pDecl->AppendAttribute(wolf::AT_Tangent, 3, wolf::CT_Float, pInterleavedMesh->tangentOffset);

		if( pInterleavedMesh->bitangentOffset >= 0 )
			pDecl->AppendAttribute(wolf::AT_BiTangent, 3, wolf::CT_Float, pInterleavedMesh->bitangentOffset);

		if( pInterleavedMesh->numColorChannels > 0 )
			pDecl->AppendAttribute(wolf::AT_Color, 4, wolf::CT_UByte, pInterleavedMesh->pColorOffsets[0]);

		for( int x = 0; x < pInterleavedMesh->numUVChannels; x++ )
			pDecl->AppendAttribute((wolf::Attribute)(wolf::AT_TexCoord1 + x), pInterleavedMesh->pUVSizes[x], wolf::CT_Float, pInterleavedMesh->pUVOffsets[x]);

		pDecl->SetVertexBuffer(pVB);
		pDecl->SetIndexBuffer(pIB);
		pDecl->End(pInterleavedMesh->vertexStride);

		// Add them to our list of meshes
		Mesh m;
		m.m_pVB = pVB;
		m.m_pIB = pIB;
		m.m_pDecl = pDecl;
		m.m_numTriangles = pMesh->mNumFaces;
        m.m_minPos = pInterleavedMesh->boundsMin;
        m.m_maxPos = pInterleavedMesh->boundsMax;
		m_meshes.push_back(m);
	}

    _addMeshesFromNode(pScene, pScene->mRootNode, aiMatrix4x4(), matProvider);
}

//----------------------------------------------------------
// Destructor
//----------------------------------------------------------
Model::~Model()
{
	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		const Mesh& m = m_meshes[i];
		wolf::BufferManager::DestroyBuffer(m.m_pVB);
		wolf::BufferManager::DestroyBuffer(m.m_pIB);
		delete m.m_pDecl;
	}

	for(unsigned int i = 0; i < m_meshes.size(); i++)
    {
        const MeshNode& meshNode = m_meshNodes[i];
        if(meshNode.m_ownMaterial)
            wolf::MaterialManager::DestroyMaterial(meshNode.m_pMat);
    }
}

//----------------------------------------------------------
// Renders this model
//----------------------------------------------------------
void Model::Render(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
	for(int i = 0; i < m_meshNodes.size(); i++)
	{
		const MeshNode& m = m_meshNodes[i];
        glm::mat4 world = worldMatrix * m.m_transform;

		m.m_pMat->SetUniform("projection", projMatrix);
		m.m_pMat->SetUniform("view", viewMatrix);
		m.m_pMat->SetUniform("world", world);
		m.m_pMat->SetUniform("worldIT", glm::transpose(glm::inverse(world)));
		m.m_pMat->Apply();

		m.m_pMesh->m_pDecl->Bind();
		glDrawElements(GL_TRIANGLES, m.m_pMesh->m_numTriangles * 3, GL_UNSIGNED_SHORT, 0);
	}
}

void Model::OverrideMaterial(wolf::Material *pMat)
{
	for(int i = 0; i < m_meshNodes.size(); i++)
	{
		MeshNode& m = m_meshNodes[i];

        if(m.m_ownMaterial)
            wolf::MaterialManager::DestroyMaterial(m.m_pMat);

        m.m_ownMaterial = false;
		m.m_pMat = pMat;
	}
}

int Model::_getInterleavedVertexSize(const aiMesh* pMesh)
{
    int total = sizeof(aiVector3D); // assuming presence of positions
    // Only support one color channel right now (to keep wolf simple)
    total += (pMesh->GetNumColorChannels() > 0 ? 1 : 0) * 4;
    total += pMesh->HasNormals() ? sizeof(aiVector3D) : 0;
    total += pMesh->HasTangentsAndBitangents() ? sizeof(aiVector3D) * 2 : 0;

    for(unsigned int i = 0; i < pMesh->GetNumUVChannels(); ++i)
    {
        total += pMesh->mNumUVComponents[i] * sizeof(float);
    }

    return total;
}

Model::InterleavedMesh* Model::_interleaveMeshData(const aiMesh* pMesh)
{
    glm::vec3 minPos(1000000.0f, 1000000.0f, 1000000.0f);
    glm::vec3 maxPos(-1000000.0f, -1000000.0f, -1000000.0f);

    Model::InterleavedMesh* pOutMesh = new Model::InterleavedMesh();
    pOutMesh->vertexStride = _getInterleavedVertexSize(pMesh);
    pOutMesh->interleavedDataSize = pOutMesh->vertexStride * pMesh->mNumVertices;
    pOutMesh->pInterleavedData = new unsigned char[pOutMesh->interleavedDataSize];
    pOutMesh->numIndices = pMesh->mNumFaces * 3;
    pOutMesh->pIndexData = new unsigned short[pOutMesh->numIndices];

    unsigned char* pCursor = (unsigned char*)pOutMesh->pInterleavedData;
    int vertOffset = 0;

    for(unsigned int i = 0; i < pMesh->mNumVertices; ++i)
    {
        bool firstVert = i == 0;
        aiVector3D* pPos = (aiVector3D*)pCursor;
        *pPos = pMesh->mVertices[i];
        pCursor += sizeof(aiVector3D);

        if(pPos->x > maxPos.x)
            maxPos.x = pPos->x;
        else if(pPos->x < minPos.x)
            minPos.x = pPos->x;

        if(pPos->y > maxPos.y)
            maxPos.y = pPos->y;
        else if(pPos->y < minPos.y)
            minPos.y = pPos->y;

        if(pPos->z > maxPos.z)
            maxPos.z = pPos->z;
        else if(pPos->z < minPos.z)
            minPos.z = pPos->z;

        if(firstVert)
        {
            pOutMesh->posOffset = vertOffset;
            vertOffset += sizeof(aiVector3D);
        }

        if(pMesh->HasNormals())
        {
            aiVector3D* pNormal = (aiVector3D*)pCursor;
            *pNormal = pMesh->mNormals[i];
            pCursor += sizeof(aiVector3D);

            if(firstVert)
            {
                pOutMesh->normalOffset = vertOffset;
                vertOffset += sizeof(aiVector3D);
            }
        }

        if(pMesh->HasTangentsAndBitangents())
        {
            aiVector3D* pTangent = (aiVector3D*)pCursor;
            *pTangent = pMesh->mTangents[i];
            pCursor += sizeof(aiVector3D);

            aiVector3D* pBiTangent = (aiVector3D*)pCursor;
            *pBiTangent = pMesh->mBitangents[i];
            pCursor += sizeof(aiVector3D);

            if(firstVert)
            {
                pOutMesh->tangentOffset = vertOffset;
                vertOffset += sizeof(aiVector3D);
                pOutMesh->bitangentOffset = vertOffset;
                vertOffset += sizeof(aiVector3D);
            }
        }

        // only support one color channel right now (would require changing the "push" attr location mapping
        // to "pull" which complicates early learning)
        const int numColorChannels = pMesh->GetNumColorChannels() > 0 ? 1 : 0;

        for(int c = 0; c < numColorChannels; ++c)
        {
            pCursor[0] = (unsigned char) (pMesh->mColors[c][i].r * 255.0f);
            pCursor[1] = (unsigned char) (pMesh->mColors[c][i].g * 255.0f);
            pCursor[2] = (unsigned char) (pMesh->mColors[c][i].b * 255.0f);
            pCursor[3] = (unsigned char) (pMesh->mColors[c][i].a * 255.0f);
            pCursor += 4;

            if(firstVert)
            {
                if( c == 0)
                {
                    pOutMesh->numColorChannels = pMesh->GetNumColorChannels();
                    pOutMesh->pColorOffsets = new int[pOutMesh->numColorChannels];
                }

                pOutMesh->pColorOffsets[c] = vertOffset;
                vertOffset += 4;
            }
        }

        for(unsigned int uvSet = 0; uvSet < pMesh->GetNumUVChannels(); ++uvSet)
        {
            if(firstVert)
            {
                if( uvSet == 0)
                {
                    pOutMesh->numUVChannels = pMesh->GetNumUVChannels();
                    pOutMesh->pUVOffsets = new int[pOutMesh->numUVChannels];
                    pOutMesh->pUVSizes = new int[pOutMesh->numUVChannels];
                }
            }

            float* pUV = (float*)pCursor;
            switch(pMesh->mNumUVComponents[uvSet])
            {
                case 1:
                    pUV[0] = pMesh->mTextureCoords[uvSet][i].x;
                    pCursor += sizeof(float);
                    if(firstVert)
                    {
                        pOutMesh->pUVOffsets[uvSet] = vertOffset;
                        pOutMesh->pUVSizes[uvSet] = 1;
                        vertOffset += sizeof(float);
                    }
                    break;
                case 2:
                    pUV[0] = pMesh->mTextureCoords[uvSet][i].x;
                    pUV[1] = pMesh->mTextureCoords[uvSet][i].y;
                    pCursor += sizeof(float) * 2;
                    if(firstVert)
                    {
                        pOutMesh->pUVOffsets[uvSet] = vertOffset;
                        pOutMesh->pUVSizes[uvSet] = 2;
                        vertOffset += sizeof(float) * 2;
                    }
                    break;
                case 3:
                    pUV[0] = pMesh->mTextureCoords[uvSet][i].x;
                    pUV[1] = pMesh->mTextureCoords[uvSet][i].y;
                    pUV[2] = pMesh->mTextureCoords[uvSet][i].z;
                    pCursor += sizeof(float) * 3;
                    if(firstVert)
                    {
                        pOutMesh->pUVOffsets[uvSet] = vertOffset;
                        pOutMesh->pUVSizes[uvSet] = 3;
                        vertOffset += sizeof(float) * 3;
                    }
                    break;
            }
        }
    }

    for(unsigned int i = 0; i < pMesh->mNumFaces; ++i)
    {
        aiFace* pFace = &pMesh->mFaces[i];
        assert(pFace->mNumIndices == 3);
        pOutMesh->pIndexData[i*3+0] = pFace->mIndices[0];
        pOutMesh->pIndexData[i*3+1] = pFace->mIndices[1];
        pOutMesh->pIndexData[i*3+2] = pFace->mIndices[2];
    }

    pOutMesh->boundsMin = minPos;
    pOutMesh->boundsMax = maxPos;
    return pOutMesh;
}

glm::vec3 Model::getAABBMin() const
{
    glm::vec3 minPos(1000000.0f, 1000000.0f, 1000000.0f);
    for(int i = 0; i < m_meshes.size(); ++i)
    {
        const Mesh& mesh = m_meshes[i];

        if(mesh.m_minPos.x < minPos.x)
            minPos.x = mesh.m_minPos.x;

        if(mesh.m_minPos.y < minPos.y)
            minPos.y = mesh.m_minPos.y;

        if(mesh.m_minPos.z < minPos.z)
            minPos.z = mesh.m_minPos.z;
    }
    return minPos;
}

glm::vec3 Model::getAABBMax() const
{
    glm::vec3 maxPos(-1000000.0f, -1000000.0f, -1000000.0f);
    for(int i = 0; i < m_meshes.size(); ++i)
    {
        const Mesh& mesh = m_meshes[i];

        if(mesh.m_maxPos.x > maxPos.x)
            maxPos.x = mesh.m_maxPos.x;

        if(mesh.m_maxPos.y > maxPos.y)
            maxPos.y = mesh.m_maxPos.y;

        if(mesh.m_maxPos.z > maxPos.z)
            maxPos.z = mesh.m_maxPos.z;
    }
    return maxPos;
}

glm::vec3 Model::getAABBCenter() const
{
    glm::vec3 min = getAABBMin();
    glm::vec3 max = getAABBMax();
    return min + ((max - min) * 0.5f);
}

void Model::_addMeshesFromNode(const aiScene* pScene, const aiNode* pNode, const aiMatrix4x4& parentTransform, const MaterialProvider& matProvider)
{
    aiMatrix4x4 t = pNode->mTransformation * parentTransform;

    for(unsigned int i = 0; i < pNode->mNumMeshes; ++i)
    {
        int meshIdx = pNode->mMeshes[i];
        aiMesh* pMesh = pScene->mMeshes[meshIdx];
        
        MeshNode meshNode;
        meshNode.m_pMesh = &m_meshes[meshIdx];
        meshNode.m_transform = glm::mat4( t.a1,t.b1,t.c1,t.d1,
                                          t.a2,t.b2,t.c2,t.d2,
                                          t.a3,t.b3,t.c3,t.d3,
                                          t.a4,t.b4,t.c4,t.d4);

        aiString matName;
        aiMaterial* pMat = pScene->mMaterials[pMesh->mMaterialIndex];
		pMat->Get(AI_MATKEY_NAME, matName);

        meshNode.m_pMat = matProvider.getMaterial(pNode->mName.C_Str(), i, matName.C_Str()); 
        meshNode.m_ownMaterial = false;

        if(!meshNode.m_pMat)
        {
            if(m_createdMaterials.find(pMesh->mMaterialIndex) == m_createdMaterials.end())
                m_createdMaterials[pMesh->mMaterialIndex] = _createMaterialFromAssimpMaterial(pMat);

            meshNode.m_pMat = m_createdMaterials[pMesh->mMaterialIndex];
            meshNode.m_ownMaterial = true;
        }

        m_meshNodes.push_back(meshNode);
    }

    for(unsigned int i = 0; i < pNode->mNumChildren; ++i)
    {
        _addMeshesFromNode(pScene, pNode->mChildren[i], t, matProvider);
    }
}

wolf::Color4 _getColorRGBA(const aiMaterial* pMat, 
                           const char* key, 
                           int type, 
                           int idx, 
                           float defaultR = 1.0f, 
                           float defaultG = 1.0f, 
                           float defaultB = 1.0f, 
                           float defaultA = 1.0f)
{
    aiColor3D c(defaultR,defaultG,defaultB);
    pMat->Get(key, type, idx, c);
    return wolf::Color4(c.r, c.g, c.b, 1.0f);
}

float _getFloat(const aiMaterial* pMat, const char* key, int type, int idx, float defaultVal = 0.0f)
{
    float f = defaultVal;
    pMat->Get(key, type, idx, f);
    return f;
}

wolf::Texture* _getTexture(const aiMaterial* pMat, aiTextureType texType)
{
    aiString path; 
    aiReturn ret = pMat->GetTexture(texType, 0, &path);
    if (ret == AI_SUCCESS)
        return wolf::TextureManager::CreateTexture(std::string("data/") + path.C_Str());

    return nullptr;
}

wolf::Material* Model::_createMaterialFromAssimpMaterial(const aiMaterial* pAssimpMat)
{
    aiString name;
    pAssimpMat->Get(AI_MATKEY_NAME,name);
    wolf::Material *pMat = wolf::MaterialManager::CreateMaterial(m_path + "/" + name.C_Str());
    pMat->SetProgram("data/uber.vsh", "data/uber.fsh");
    pMat->SetDepthTest(true);
    pMat->SetDepthWrite(true);

    wolf::Texture* pAlbedo = _getTexture(pAssimpMat, aiTextureType_DIFFUSE);
    if(pAlbedo)
    {
        pMat->EnableKeyword("ALBEDO_TEX");
        pMat->SetTexture("u_albedoTex", pAlbedo);
    }
    else 
    {
        pMat->SetUniform("u_albedo", _getColorRGBA(pAssimpMat, AI_MATKEY_COLOR_DIFFUSE));
    }

    // TODO
    // aiColor3D specularColor = _getColorRGB(pAssimpMat, AI_MATKEY_COLOR_SPECULAR);
    // aiColor3D emissiveColor = _getColorRGB(pAssimpMat, AI_MATKEY_COLOR_EMISSIVE);
    // Specular strength:
    //   float specStrength = 2.0f * _getFloat(pAssimpMat,AI_MATKEY_SHININESS_STRENGTH);
    // Roughness:
    //   float roughness = 1.0f - (_getFloat(pAssimpMat,AI_MATKEY_REFLECTIVITY / 100.0f);
    // Opacity
    //   float alpha = _getFloat(pAssimpMat,AI_MATKEY_OPACITY);

    // Textures TODO
    // Using Blender's "Principled BSDF", we get the following mappings in Assimp:
    //   Specular  = aiTextureType_SPECULAR
    //   Roughness  = aiTextureType_SHININESS
    //   Emission  = aiTextureType_EMISSIVE
    //   Normal  = aiTextureType_NORMALS

    return pMat;
}

}