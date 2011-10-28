#ifndef COCAINE_BACKENDS_PROCESS_HPP
#define COCAINE_BACKENDS_PROCESS_HPP

#include "cocaine/backends/abstract.hpp"

namespace cocaine { namespace engine {

// Applcation Engine Worker
class process_t:
    public backend_t
{
    public:        
        process_t(boost::shared_ptr<engine_t> parent,
                  const std::string& type,
                  const std::string& args);
        virtual ~process_t();

    private:
        virtual void timeout(ev::timer& w, int revents);
        void signal(ev::child& w, int revents);

    private:
        boost::shared_ptr<engine_t> m_parent;
        pid_t m_pid;

        ev::child m_child_watcher;
};

}}

#endif