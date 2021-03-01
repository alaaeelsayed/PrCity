//-----------------------------------------------------------------------------
// File:			W_Model.h
// Original Author:	Gordon Wood
//
// Class representing a model that can be loaded from an FBX file
//-----------------------------------------------------------------------------
#ifndef W_MODEL_H
#define W_MODEL_H

#include "W_Types.h"
#include "W_VertexBuffer.h"
#include "W_IndexBuffer.h"
#include "W_BufferManager.h"
#include "W_VertexDeclaration.h"
#include "W_TextureManager.h"
#include "W_ProgramManager.h"
#include "W_Material.h"
#include <string>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace wolf
{
class Model
{
	public:
		//-------------------------------------------------------------------------
		// PUBLIC INTERFACE
		//-------------------------------------------------------------------------
		class MaterialProvider
		{
		public:
			// Return nullptr to have the model automatically create its own material from the data in the imported format
			virtual wolf::Material* getMaterial(const std::string& nodeName, int subMeshIndex, const std::string& name) const 
			{ 
				return nullptr; 
			}
		};

		Model(const std::string& path, const MaterialProvider& matProvider = MaterialProvider());
		~Model();

		glm::vec3 getAABBMin() const;
		glm::vec3 getAABBMax() const;
		glm::vec3 getAABBCenter() const;

		void Render(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

		// Used for multi-pass samples. Really, the material should support multiple passes but we don't have that yet
		void OverrideMaterial(wolf::Material* pMat);
		//-------------------------------------------------------------------------

	private:
		//-------------------------------------------------------------------------
		// PRIVATE TYPES
		//-------------------------------------------------------------------------
		struct Mesh
		{
			wolf::VertexBuffer* m_pVB;
			wolf::IndexBuffer* m_pIB;
			wolf::VertexDeclaration* m_pDecl;
			int m_numTriangles;
			glm::vec3 m_minPos;
			glm::vec3 m_maxPos;
		};

		struct MeshNode
		{
			Mesh* m_pMesh = nullptr;
			glm::mat4 m_transform;
			wolf::Material* m_pMat = nullptr;
			bool m_ownMaterial = false;
		};

		struct InterleavedMesh
		{
			int numVertices = 0;
			int posOffset = -1;
			int normalOffset = -1;
			int tangentOffset = -1;
			int bitangentOffset = -1;
			int *pColorOffsets = nullptr;
			int numColorChannels;
			int *pUVOffsets = nullptr;
			int *pUVSizes = nullptr;
			int numUVChannels = 0;
			glm::vec3 boundsMax;
			glm::vec3 boundsMin;

			unsigned char *pInterleavedData = nullptr;
			unsigned short *pIndexData = nullptr;
			int interleavedDataSize = 0;
			int vertexStride = 0;
			int numIndices = 0;

			~InterleavedMesh()
			{
				delete[] pInterleavedData;
				delete[] pIndexData;
				pInterleavedData = nullptr;
				if (pUVOffsets)
					delete[] pUVOffsets;
				if (pUVSizes)
					delete[] pUVSizes;
				if (pColorOffsets)
					delete[] pColorOffsets;
			}
		};
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE METHODS
		//-------------------------------------------------------------------------
		InterleavedMesh* _interleaveMeshData(const aiMesh* pMesh);
		int _getInterleavedVertexSize(const aiMesh* pMesh);
		void _addMeshesFromNode(const aiScene* pScene, const aiNode* pNode, const aiMatrix4x4& parentTransform, const MaterialProvider& matProvider);
		wolf::Material* _createMaterialFromAssimpMaterial(const aiMaterial* pAssimpMat);
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE MEMBERS
		//-------------------------------------------------------------------------
		std::vector<Mesh>		m_meshes;
		std::vector<MeshNode>	m_meshNodes;
		std::string 			m_path;

		std::map<int, wolf::Material*> m_createdMaterials;
		//-------------------------------------------------------------------------
};

}

#endif



