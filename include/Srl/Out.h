#ifndef SRL_OUT_H
#define SRL_OUT_H

#include "Heap.h"
#include "Blocks.h"

#include <ostream>

namespace Srl { namespace Lib {

    class Out {

    public:
        struct Source {
            Source(std::vector<uint8_t>& vec) : buffer(&vec), is_stream(false) { }
            Source(std::ostream& strm) : stream(&strm), is_stream(true) { }
            union {
                std::ostream* stream;
                std::vector<uint8_t>* buffer;
            };
            bool is_stream;
        };

        Out() { }

        class Ticket;

        template<class T> void write (const T& o);
        inline void write_byte (uint8_t byte);
        inline void write (const MemBlock& block);
        inline void write (const uint8_t* bytes, size_t nbytes);
        inline void write (Ticket& ticket, const uint8_t* bytes, size_t offset, size_t nbytes);
        inline void write_times (size_t n_times, uint8_t byte);

        template<class... Tokens>
        void write_substitute(const Lib::MemBlock& data, const Tokens&... tokens);

        void            flush();
        inline Ticket   reserve (size_t nbytes);

        void set(Source source);

        class Ticket {

        friend class Out;
        public:
            Ticket() { }
        private:
            Ticket(uint8_t* mem_, size_t size_, size_t pos_, size_t seg_id_)
                : mem(mem_), size(size_), pos(pos_), seg_id(seg_id_) { }

            uint8_t* mem;
            size_t   size;
            size_t   pos;
            size_t   seg_id;
        };

    private:
        static const size_t Stream_Buffer_Size  = 1024;

        Heap   heap;

        struct State {
            size_t sz_total     = 0;
            size_t left         = 0;
            size_t cap          = 0;
            size_t segs_flushed = 0;
            uint8_t* crr_mem   = nullptr;
            uint8_t* mem_start = nullptr;
        } state;

        bool streaming;
        std::ostream* stream = nullptr;
        std::vector<uint8_t>* buffer = nullptr;

        void inc_cap          (size_t nbytes);
        inline uint8_t* alloc (size_t nbytes);
        void write_to_stream  ();
        void write_to_buffer  ();

        template<class Token, class Sub, class... Tokens>
        void substitute_token(uint8_t src, const Token& token, const Sub& sub, const Tokens&... tail);

        inline void substitute_token(uint8_t src);

        template<size_t N, class Sub> typename std::enable_if<N != 0, void>::type
        write_substitute(uint8_t* dest, const Sub* sub);

        template<size_t N, class Sub> typename std::enable_if<N == 0, void>::type
        write_substitute(uint8_t*, const Sub*) { }
    };
} }

#endif
