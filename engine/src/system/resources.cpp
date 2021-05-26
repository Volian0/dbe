#include <string.h>

#include "resources.hpp"
#include "logger.hpp"

#include <physfs.h>

std::shared_ptr <Resource> Resource::make_text(const std::string& text) {
	auto r = std::make_shared<Resource>();
	r->type = TEXT;

	char* buffer = (char*)malloc(text.size() + 1);
	memcpy(buffer, text.c_str(), text.size() + 1);
	buffer[text.size()] = '\0';

	r->data = buffer;
	r->size = text.size();
	r->modtime = 0;

	return r;
}

Resource::~Resource() {
	switch (type) {
	case TEXT:
	case BINARY:
		free(data);
		break;
	};

	data = nullptr;
	size = 0;
}

void ResourceManager::init(const std::string& working_directory) {
	auto& i = instance();

	i.m_working_directory = working_directory;

	if (PHYSFS_isInit()) { /* In-case the user is trying to re-init */
		quit();
	}

	PHYSFS_init(nullptr);
	PHYSFS_mount(working_directory.c_str(), "/", 1);
	PHYSFS_setWriteDir(working_directory.c_str());
}

void ResourceManager::quit() {
	auto& i = instance();

	i.m_cache.clear();

	PHYSFS_unmount(i.m_working_directory.c_str());

	PHYSFS_deinit();
}

std::string ResourceManager::get_file_extension(const std::string& name) {
	auto i = name.find_last_of('.');

	std::string ext;

	if (i != std::string::npos) {
		ext = name.substr(i + 1);
	}

	return ext;
}

const std::shared_ptr <Resource>& ResourceManager::load_text(const std::string& name, bool reload) {
	auto& i = instance();

	if (i.m_cache.count(name) != 0) {
		if (i.m_cache[name]->type == Resource::TEXT && !reload) {
			return i.m_cache[name];
		} else {
			/* The types are somehow different, so to be
			 * safe, we refresh the file. */
			i.m_cache[name].reset();
		}
	}

	PHYSFS_file* file = PHYSFS_openRead(name.c_str());
	if (file == NULL) {
		log(LOG_ERROR, "Failed to load text file `%s'", name.c_str());
		i.m_cache[name] = Resource::make_text(" ");;
		return i.m_cache[name];
	}

	PHYSFS_Stat stat;
	PHYSFS_stat(name.c_str(), &stat);

	u32 file_size = PHYSFS_fileLength(file);

	char* buffer = (char*)malloc(file_size + 1);

	u32 bytes_read = PHYSFS_readBytes(file, buffer, file_size);
	buffer[bytes_read] = '\0';

	auto r = std::make_shared<Resource>();
	r->type = Resource::TEXT;
	r->data = buffer;
	r->size = file_size;
	r->modtime = stat.modtime;

	i.m_cache[name] = r;

	return i.m_cache[name];
}

const std::shared_ptr <Resource>& ResourceManager::load_binary(const std::string& name, bool reload) {
	auto& i = instance();

	if (i.m_cache.count(name) != 0) {
		if (i.m_cache[name]->type == Resource::TEXT && !reload) {
			return i.m_cache[name];
		} else {
			/* The types are somehow different, so to be
			 * safe, we refresh the file. */
			i.m_cache[name].reset();
		}
	}

	PHYSFS_file* file = PHYSFS_openRead(name.c_str());
	if (file == NULL) {
		log(LOG_ERROR, "Failed to load text file `%s'", name.c_str());
		i.m_cache[name] = Resource::make_text(" ");;
		return i.m_cache[name];
	}

	PHYSFS_Stat stat;
	PHYSFS_stat(name.c_str(), &stat);

	u32 file_size = PHYSFS_fileLength(file);

	void* buffer = malloc(file_size);

	PHYSFS_readBytes(file, buffer, file_size);

	auto r = std::make_shared<Resource>();
	r->type = Resource::BINARY;
	r->data = buffer;
	r->size = file_size;
	r->modtime = stat.modtime;

	i.m_cache[name] = r;

	return i.m_cache[name];
}

std::string ResourceManager::load_string(const std::string& name, bool reload) {
	return load_text(name, reload)->as_text();
}

void ResourceManager::hot_reload() {
	auto& i = instance();

	for (auto& f : i.m_cache) {
		PHYSFS_Stat stat;
		PHYSFS_stat(f.first.c_str(), &stat);

		if (stat.modtime > f.second->modtime) {
			load_text(f.first, true);
		}
	}
}
