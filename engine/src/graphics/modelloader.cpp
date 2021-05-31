#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "modelloader.hpp"
#include "int.hpp"
#include "logger.hpp"
#include "system/transform.hpp"
#include "system/resources.hpp"

std::string current_shader;

void process_mesh(Scene& scene, Entity* parent, aiMesh* mesh, aiNode* node, const aiScene* model) {
	aiVector3D translation, rotation, scale;
	node->mTransformation.Decompose(scale, rotation, translation);

	Entity entity = scene.new_entity();
	entity.add_component<Transform>({
		{translation.x, translation.y, translation.z},
		{rotation.x, rotation.y, rotation.z},
		{scale.x, scale.y, scale.z},
	});
	if (parent) {
		entity.parent_to(parent);
	}

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

	vec3 lit_color = {0.788, 0.176, 0.133};
	vec3 unlit_color = {0.388, 0.141, 0.121};
	vec3 specular_color = {0.886, 0.557, 0.533};

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = model->mMaterials[mesh->mMaterialIndex];

		aiColor3D color_diffuse;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color_diffuse);
		lit_color = vec3(
			color_diffuse.r,
			color_diffuse.g,
			color_diffuse.b
		);

		unlit_color = vec3(
			color_diffuse.r - 0.3,
			color_diffuse.g - 0.3,
			color_diffuse.b - 0.3
		);

		if (unlit_color.x <= 0.0) { unlit_color.x = 0.0; }
		if (unlit_color.y <= 0.0) { unlit_color.y = 0.0; }
		if (unlit_color.z <= 0.0) { unlit_color.z = 0.0; }

		specular_color = vec3(
			color_diffuse.r + 0.3,
			color_diffuse.g + 0.3,
			color_diffuse.b + 0.3
		);

		if (unlit_color.x >= 1.0) { unlit_color.x = 1.0; }
		if (unlit_color.y >= 1.0) { unlit_color.y = 1.0; }
		if (unlit_color.z >= 1.0) { unlit_color.z = 1.0; }
	}

	entity.add_component<Shader>(scene.m_renderer->get_shader(current_shader));
	entity.add_component<Mesh>(scene.m_renderer->new_mesh(
		mesh->mName.C_Str(), Mesh::Flags::DRAW_TRIANGLES, vertices, indices, mlc));
	entity.add_component<Material>({
		lit_color, /* lit color */
		unlit_color, /* unlit color */
		specular_color, /* specular color */
		0.9, /* specular_cutoff */
	});
}

void process_node(Scene& scene, Entity* parent, aiNode* node, const aiScene* model) {
	for (u32 i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = model->mMeshes[node->mMeshes[i]];
		process_mesh(scene, parent, mesh, node, model);
	}

	for (u32 i = 0; i < node->mNumChildren; i++) {
		aiVector3D translation, rotation, scale;
		node->mTransformation.Decompose(scale, rotation, translation);

		Entity entity = scene.new_entity();
		entity.add_component<Transform>({
			{translation.x, translation.y, translation.z},
			{rotation.x, rotation.y, rotation.z},
			{scale.x, scale.y, scale.z},
		});
		if (parent) {
			entity.parent_to(parent);
		}

		process_node(scene, &entity, node->mChildren[i], model);
	}
}

Entity load_model(Scene& scene, const std::string& path, const std::string& shader) {
	auto resource = ResourceManager::load_binary(path);

	Assimp::Importer import;
	const aiScene* model =
		import.ReadFileFromMemory(resource->data, resource->size,
			aiProcess_Triangulate | aiProcess_FlipUVs,
			ResourceManager::get_file_extension(path).c_str());

	if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode) {
		log(LOG_ERROR, "Error loading model: %s", import.GetErrorString());
		return Entity {};
	}

	current_shader = shader;

	Entity entity = scene.new_entity();
	entity.add_component<Transform>({
		{0.0, 0.0, 0.0}, /* translation */
		{0.0, 0.0, 0.0}, /* rotation */
		{1.0, 1.0, 1.0}, /* scale */
	});

	process_node(scene, &entity, model->mRootNode, model);
	return entity;
}
