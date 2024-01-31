#pragma once

#include <ostream>

namespace rb::fmt {

template <class Stream>
class StreamStateSaver final {
public:
	explicit StreamStateSaver(Stream& stream)
	    : stream_(stream)
	    , flags_(stream.flags())
	    , precision_(stream.precision())
	    , fillCh_(stream.fill()) {
	}

	~StreamStateSaver() {
		stream_.flags(flags_);
		stream_.precision(precision_);
		stream_.fill(fillCh_);
	}

private:
	Stream& stream_;
	std::ios_base::fmtflags flags_;
	std::streamsize precision_;
	typename Stream::char_type fillCh_;
};

} // namespace rb::fmt
