// (C) Copyright Jonathan Turkanis 2004
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef BOOST_IOSTREAMS_FILTER_STREAM_HPP_INCLUDED
#define BOOST_IOSTREAMS_FILTER_STREAM_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <boost/iostreams/detail/disable_warnings.hpp>  // MSVC.

#include <istream>
#include <memory>                                // allocator.
#include <ostream>
#include <boost/iostreams/detail/access_control.hpp>
#include <boost/iostreams/detail/push.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/type_traits/is_convertible.hpp>

namespace boost { namespace iostreams {

//--------------Definition of filtered_istream--------------------------------//

namespace detail {

template<typename Mode, typename Ch, typename Tr>
struct filtering_stream_traits {
    typedef typename 
            select<                 
                mpl::and_< 
                    is_convertible<Mode, input>, 
                    is_convertible<Mode, output> 
                >,          
                std::basic_iostream<Ch, Tr>,
                is_convertible<Mode, input>, 
                std::basic_istream<Ch, Tr>,
                mpl::true_,        
                std::basic_ostream<Ch, Tr>
            >::type                               type;
};

template<typename Chain, typename Access>
class filtering_stream_base 
    : public access_control<
                 boost::iostreams::detail::chain_client<Chain>,
                 Access
             >,
      public filtering_stream_traits<
                 typename Chain::mode, 
                 typename Chain::char_type, 
                 typename Chain::traits_type
             >::type
{
public:
    typedef Chain                                         chain_type;
    typedef access_control<
                 boost::iostreams::detail::chain_client<Chain>,
                 Access
             >                                            client_type;
protected:
    typedef typename 
            filtering_stream_traits<
                 typename Chain::mode, 
                 typename Chain::char_type, 
                 typename Chain::traits_type
            >::type                                       stream_type;
    filtering_stream_base() : stream_type(0) { this->set_chain(&chain_); }
private:
    void notify() { this->rdbuf(chain_.empty() ? 0 : &*chain_); }
    Chain chain_;
};

} // End namespace detail.

//
// Macro: BOOST_IOSTREAMS_DEFINE_FILTER_STERAM(name_, chain_type_, default_char_)
// Description: Defines a template derived from std::basic_streambuf which uses
//      a chain to perform i/o. The template has the following parameters:
//      Ch - The character type.
//      Tr - The character traits type.
//      Alloc - The allocator type.
//      Access - Indicates accessibility of the chain interface; must be either
//          public_ or protected_; defaults to public_.
// Macro parameters:
//      name_ - The name of the template to be defined.
//      mode_ - The i/o mode of the template to be defined.
//
#define BOOST_IOSTREAMS_DEFINE_FILTER_STREAM(name_, chain_type_, default_char_) \
    template< typename Mode, \
              typename Ch = default_char_, \
              typename Tr = std::char_traits<Ch>, \
              typename Alloc = std::allocator<Ch>, \
              typename Access = public_ > \
    class name_ \
        : public boost::iostreams::detail::filtering_stream_base< \
                     chain_type_<Mode, Ch, Tr, Alloc>, Access \
                 > \
    { \
    public: \
        typedef Ch                                char_type; \
        BOOST_IOSTREAMS_STREAMBUF_TYPEDEFS(Tr) \
        typedef Mode                              mode; \
        typedef chain_type_<Mode, Ch, Tr, Alloc>  chain_type; \
        name_() { } \
        BOOST_IOSTREAMS_DEFINE_PUSH_CONSTRUCTOR(mode, Ch, name_, push_impl) \
        ~name_() { if (this->is_complete()) this->rdbuf()->pubsync(); } \
        void clear(std::ios::iostate state = std::ios::goodbit) \
        { std::basic_ios<char_type, traits_type>::clear(state); } \
    private: \
        typedef access_control< \
                    boost::iostreams::detail::chain_client< \
                        chain_type_<Mode, Ch, Tr, Alloc> \
                    >, \
                    Access \
                > client_type; \
        template<typename T> \
        void push_impl(const T& t BOOST_IOSTREAMS_PUSH_PARAMS()) \
        { client_type::push(t BOOST_IOSTREAMS_PUSH_ARGS()); } \
    }; \
    /**/    
BOOST_IOSTREAMS_DEFINE_FILTER_STREAM(filtering_stream, boost::iostreams::detail::chain, char)
BOOST_IOSTREAMS_DEFINE_FILTER_STREAM(wfiltering_stream, boost::iostreams::detail::chain, wchar_t)  

typedef filtering_stream<input>    filtering_istream;
typedef filtering_stream<output>   filtering_ostream;
typedef wfiltering_stream<input>   filtering_wistream;
typedef wfiltering_stream<output>  filtering_wostream;

//----------------------------------------------------------------------------//

} } // End namespace iostreams, boost

#include <boost/iostreams/detail/enable_warnings.hpp> // MSVC

#endif // #ifndef BOOST_IOSTREAMS_FILTER_STREAM_HPP_INCLUDED