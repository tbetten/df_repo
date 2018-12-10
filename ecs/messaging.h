#pragma once
#include <memory>
#include <functional>
#include <any>
/*
struct Payload_base
{
	virtual ~Payload_base () = default;
};

using Payload = std::shared_ptr<Payload_base>;*/
using Callback = std::function<void (std::any)>;

class Dispatcher
{
public:
	void bind (Callback c)
	{
		m_callbacks.push_back (c);
	}
	void notify (std::any value) const
	{
		for (auto callback : m_callbacks)
		{
			callback (value);
		}
	}
private:
	std::vector<Callback> m_callbacks;
};