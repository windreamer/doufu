#ifndef __MINITRANS_HPP__
#define __MINITRANS_HPP__
#include <inttypes.h>
#include <map>
#include <msgpack.hpp>
#include <set>
#include <string>

namespace doufu
{
    typedef uint8_t result_type;
    static const result_type OK = 0;
    static const result_type FAIL = 0;

    typedef uint8_t message_type;
    static const message_type PREPARE = 0;
    static const message_type VOTE = 0;
    static const message_type COMMIT = 0;

    typedef uint64_t id_type;

    struct prepare
    {
        prepare(id_type id_): type(PREPARE), id(id_)
        {
        }
        message_type type;
        id_type id;
        std::set<std::string> read;
        std::map<std::string, std::string> compare;
        std::map<std::string, std::string> write;
        MSGPACK_DEFINE(type, id, read, compare, write);
    };

    struct vote
    {
        vote(id_type id_): type(VOTE), id(id_)
        {
        }
        message_type type;
        id_type id;
        uint32_t barrier;
        result_type result;
        MSGPACK_DEFINE(type, id, barrier, result);
    };

    struct commit
    {
        commit(id_type id_): type(COMMIT), id(id_)
        {
        }
        message_type type;
        id_type id;
        result_type result;
        MSGPACK_DEFINE(type, id, result);
    };
}

#endif

