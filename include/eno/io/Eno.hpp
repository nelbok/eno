#pragma once

#include <eno/io/IOThread.hpp>

namespace eno {
// Eno Unit: meter
class Eno : public IOThread {
public:
	static constexpr auto fileType = "ENO file (*.eno)";
	static constexpr auto fileVersion = 1u;

	using IOThread::IOThread;

protected:
	virtual void save() override;
	virtual void load() override;
};
} // namespace eno
