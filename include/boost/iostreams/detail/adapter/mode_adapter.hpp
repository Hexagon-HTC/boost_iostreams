// (C) Copyright Jonathan Turkanis 2003.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef BOOST_IOSTREAMS_DETAIL_MODE_ADAPTER_HPP_INCLUDED
#define BOOST_IOSTREAMS_DETAIL_MODE_ADAPTER_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

// Contains the definition of the class template mode_adapter, which allows
// a filter or device to function as if it has a different i/o mode than that
// deduced by the metafunction io_mode.

#include <ios>                             // streamsize, streamoff, openmodes.
#include <boost/iostreams/categories.hpp>  // tags.
#include <boost/iostreams/traits.hpp>
#include <boost/iostreams/is_filter.hpp>
#include <boost/iostreams/operations.hpp> 
#include <boost/mpl/if.hpp> 

namespace boost { namespace iostreams { namespace detail {

template<typename T, typename Mode>
class mode_adapter {
private:
    struct empty_base { };
public:
    typedef BOOST_IOSTREAMS_CHAR_TYPE(T)                        char_type;
    struct io_category 
        : Mode, 
          device_tag,
          mpl::if_<is_filter<T>, filter_tag, device_tag>,
          mpl::if_<is_filter<T>, multichar_tag, empty_base>,
          closable_tag,
          localizable_tag
        { };
    mode_adapter(T t) : t_(t) { }

        // Device member functions.

    std::streamsize read(char_type* s, std::streamsize n);
    void write(const char_type* s, std::streamsize n);
    std::streamoff seek( std::streamoff off, std::ios::seekdir way,
                         std::ios::openmode which = 
                             std::ios::in | std::ios::out );
    void close(std::ios::openmode which = std::ios::in | std::ios::out);

        // Filter member functions.

    template<typename Source>
    std::streamsize read(Source& src, char_type* s, std::streamsize n)
    { return iostreams::read(t_, src, s, n); }

    template<typename Sink>
    void write(Sink& snk, const char_type* s, std::streamsize n)
    { return iostreams::write(t_, snk, s, n); }

    template<typename Device>
    void seek(Device& dev, std::streamoff off, std::ios::seekdir way)
    { return iostreams::seek(t_, dev, off, way); }

    template<typename Device>
    void seek( Device& dev, std::streamoff off, std::ios::seekdir way,
               std::ios::openmode which  )
    { return iostreams::seek(t_, dev, off, way, which); }

    template<typename Device>
    void close(Device& dev)
    { return iostreams::close(t_, dev); }

    template<typename Device>
    void close(Device& dev, std::ios::openmode which)
    { return iostreams::close(t_, dev, which); }

    template<typename Locale>
    void imbue(const Locale& loc)
    { return iostreams::imbue(t_, loc); }
private:
    T t_;
};
                    
//------------------Implementation of mode_adapter----------------------------//

template<typename T, typename Mode>
std::streamsize mode_adapter<T, Mode>::read(char_type* s, std::streamsize n)
{ return boost::iostreams::read(t_, s, n); }

template<typename T, typename Mode>
void mode_adapter<T, Mode>::write(const char_type* s, std::streamsize n)
{ boost::iostreams::write(t_, s, n); }

template<typename T, typename Mode>
std::streamoff mode_adapter<T, Mode>::seek
    (std::streamoff off, std::ios::seekdir way, std::ios::openmode which)
{ return boost::iostreams::seek(t_, off, way, which); }

template<typename T, typename Mode>
void mode_adapter<T, Mode>::close(std::ios::openmode which) 
{ iostreams::close(t_, which); }

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef BOOST_IOSTREAMS_DETAIL_MODE_ADAPTER_HPP_INCLUDED //-----//