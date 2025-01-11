#pragma once

namespace smse {

template<typename _Ty>
class Singleton {
public:
	static _Ty& getInstance() {
		return *getInstancePtr();
	}

	static _Ty* getInstancePtr() {
		static _Ty g_instance{};
		return &g_instance;
	}

private:
	
};

}