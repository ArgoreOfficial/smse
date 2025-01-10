#pragma once

namespace smse {

template<typename _Ty>
class Singleton {
public:
	static _Ty& getInstance() {
		static _Ty g_instance{};
		return g_instance;
	}

private:

};

}