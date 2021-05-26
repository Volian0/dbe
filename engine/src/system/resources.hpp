#pragma once

#include <string>
#include <map>
#include <memory>

#include "int.hpp"

struct Resource {
	enum Type {
		UNLOADED,
		TEXT,
		BINARY
	} type { UNLOADED };

	void* data { nullptr };

	u32 size { 0 };
	u64 modtime { 0 };

	static std::shared_ptr <Resource> make_text(const std::string& text);

	inline std::string as_text() const { return (char*)data; }

	~Resource();
};

class ResourceManager {
private:
	static inline ResourceManager& instance() {
		static ResourceManager i;
		return i;
	}

	std::string m_working_directory;

	/* Cached files, mapped by file name. */
	std::map <std::string, std::shared_ptr <Resource>> m_cache;
public:
	static void init(const std::string& working_directory);
	static void quit();

	static void hot_reload();

	static std::string get_file_extension(const std::string& name);

	static const std::shared_ptr <Resource>& load_text(const std::string& name, bool reload = false);
	static const std::shared_ptr <Resource>& load_binary(const std::string& name, bool reload = false);
	static std::string load_string(const std::string& name, bool reload = false);
};
