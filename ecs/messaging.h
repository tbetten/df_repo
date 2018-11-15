#pragma once
#include <memory>
#include <functional>

struct Payload_base
{
	virtual ~Payload_base () = default;
};

using Payload = std::shared_ptr<Payload_base>;
using Callback = std::function<void (Payload)>;

class Dispatcher
{
public:
	void bind (Callback c)
	{
		m_callbacks.push_back (c);
	}
	void notify (Payload value) const
	{
		for (auto callback : m_callbacks)
		{
			callback (value);
		}
	}
private:
	std::vector<Callback> m_callbacks;
};