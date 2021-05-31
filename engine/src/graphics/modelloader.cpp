#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "modelloader.hpp"
#include "int.hpp"
#include "logger.hpp"
#include "system/transform.hpp"

std::string current_shader;

void process_mesh(Scene& scene, Transform* parent, aiMesh* mesh, const aiScene* model) {
	Entity entity = scene.new_entity();
	entity.add_component<Transform>({
			{0.0, 0.0, 0.0}, /* translation */
			{0.0, 0.0, 0.0}, /* rotation */
			{1.0, 1.0, 1.0}, /* scale */
			parent});

	bool has_uvs = false;

	std::vector <float> vertices;
	std::vector <u32> indices;

	if (mesh->mTextureCoords[0]) {
		has_uvs = true;
	}

	for (u32 i = 0; i < mesh->mNumVertices; i++) {
		vec3 position;
		vec3 normal;
		vec2 uv;

		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;

		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;

		vertices.push_back(position.x);
		vertices.push_back(position.y);
		vertices.push_back(position.z);

		vertices.push_back(normal.x);
		vertices.push_back(normal.y);
		vertices.push_back(normal.z);

		if (has_uvs) {
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;

			vertices.push_back(uv.x);
			vertices.push_back(uv.y);
		}
	}

	for (u32 i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (u32 ii = 0; ii < face.mNumIndices; ii++) {
			indices.push_back(face.mIndices[ii]);
		}
	}

	std::vector <MeshLayoutConfig> mlc;
	if (has_uvs) {
		mlc = {
			{
				0, 3, 8, 0
			},
			{
				1, 3, 8, 3
			},
			{
				2, 2, 8, 6
			},
		};
	} else {
		mlc = {
			{
				0, 3, 6, 0
			},
			{
				1, 3, 6, 3
			}
		};
	}

	entity.add_component<Shader>(scene.m_renderer->get_shader(current_shader));
	entity.add_component<Mesh>(scene.m_renderer->new_mesh(
		mesh->mName.C_Str(), Mesh::Flags::DRAW_TRIANGLES, vertices, indices, mlc));
	entity.add_component<Material>({
		{0.788, 0.176, 0.133}, /* lit color */
		{0.388, 0.141, 0.121}, /* unlit color */
		{0.886, 0.557, 0.533}, /* specular color */
		0.9, /* specular_cutoff */
	});
}

Entity process_node(Scene& scene, Transform* parent, aiNode* node, const aiScene* model) {
	Entity entity = scene.new_entity();
	auto t = &entity.add_component<Transform>({
			{0.0, 0.0, 0.0}, /* translation */
			{0.0, 0.0, 0.0}, /* rotation */
			{1.0, 1.0, 1.0}, /* scale */
			parent});

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = model->mMeshes[node->mMeshes[i]];
		process_mesh(scene, t, mesh, model);
	}

	for (u32 i = 0; i < node->mNumChildren; i++) {
		process_node(scene, t, node->mChildren[i], model);
	}

	return entity;
}

Entity load_model(Scene& scene, const std::string& path, const std::string& shader) {
	Assimp::Importer import;
	const aiScene* model = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode) {
		log(LOG_ERROR, "Error loading model: %s", import.GetErrorString());
		return Entity {};
	}

	current_shader = shader;

	return process_node(scene, nullptr, model->mRootNode, model);
}
