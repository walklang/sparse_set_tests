
// <author>Mikhail Semenov</author>
// <date>2015-01-03</date>
// <summary>Contains implementation of Sparse Sets of Integers</summary>

#include <vector>
#include <string>
#include <iterator>
#include <limits>
#include <algorithm>

namespace bits
{
    template <class T>
    static int lsb(T x)
    {
        static const int bval[] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 }; // the first index is not used!!!

        if (x == 0)
            return -1;
        int r = 0;
        switch (sizeof(x))
        {
        case 8: if ((x & 0xFFFFFFFF) == 0) { r += 32; x >>= 32; };
        case 4: if ((x & 0x0000FFFF) == 0) { r += 16; x >>= 16; };
        case 2: if ((x & 0x000000FF) == 0) { r += 8; x >>= 8; };
        case 1: if ((x & 0x0000000F) == 0) { r += 4; x >>= 4; };
        }
        return r + bval[x & 0xF];
    }

    template<class T>
    static unsigned count_bits(T x)
    {
        static const unsigned bcount[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
        unsigned count = 0;
        while (x != 0)
        {
            count += bcount[x & 0xF];
            x >>= 4;
        }
        return count;
    }    
} // bits


///
/// Fast operations for a collection of integer values in the range [0; size-1]
/// Whenever an iterator is required an array of values is generated.
/// It outperform boost::dynamic_bitset and bounded_set (see below) in repeated iterations over the same set of values. 
/// In comparison, boost::dynamic_bitset and bounded_set use less memory and outperform the sparse set if repeated iterations are
/// not required.
///

class sparse_set
{
    static constexpr std::size_t EmptyIndex = static_cast<std::size_t>(-1);
    typedef std::size_t base_type;
    static constexpr unsigned unsigned_bits = std::numeric_limits<base_type>::digits;
    static constexpr unsigned unsigned_bits_log2 = (unsigned_bits == 64 ? 6 : unsigned_bits == 32 ? 5 : unsigned_bits == 16 ? 4 : 3);
    static constexpr unsigned unsigned_bits_log2_mask = 0xFFFFFFFF >> (32 - unsigned_bits_log2);
    static constexpr std::size_t one_bit = 1;

    unsigned m_size;
    std::vector<base_type> m_bit_array;
    mutable std::vector<std::size_t> m_sequence;
    mutable bool m_iterator_present;

    void create_iteration_sequence() const
    {
        //if (!m_sequence.empty())
        //    return;

        if (m_sequence.capacity() != m_size)
        {
            m_sequence.reserve(m_size);
        }

        std::size_t k = 0;
        for (auto x : m_bit_array)
        {
            if (x)
            {
                unsigned bit = k;
                do
                {
                    unsigned p = bits::lsb(x);
                    bit += p;
                    x >>= p;
                    m_sequence.push_back(bit);
                    if (x == 1)
                        break;
                    x >>= 1;
                    bit++;
                } while (true);
            }
            k += unsigned_bits;
        }
    }

public:
    typedef std::size_t value_type;
    typedef std::size_t key_type;
    typedef std::size_t size_type;
    typedef std::vector<std::size_t>::const_iterator iterator;
    typedef iterator const_iterator;

    typedef std::vector<std::size_t>::const_reverse_iterator reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;

    sparse_set(std::size_t size) :m_size(size),
        m_bit_array((m_size + unsigned_bits - 1) / unsigned_bits), m_sequence(), m_iterator_present(false)
    {
        
    }

    sparse_set() :m_size(0),
        m_bit_array(0), m_sequence(), m_iterator_present(false)
    {
    }

    void swap(sparse_set& s)
    {
        m_bit_array.swap(s.m_bit_array);
        m_sequence.swap(s.m_sequence);
        std::swap(m_iterator_present, s.m_iterator_present);
        std::swap(m_size, s.m_size);
    }

    void resize(std::size_t size)
    {
        m_size = size;
        m_bit_array.resize((m_size + unsigned_bits - 1) / unsigned_bits);        
        if (m_iterator_present)
        {
            m_iterator_present = false;
            m_sequence.clear();
        }
    }

    bool insert(value_type i)
    {
        if (m_iterator_present)
        {
            m_iterator_present = false;
            m_sequence.clear();
        }
        base_type& v = m_bit_array[i >> unsigned_bits_log2];
        base_type x = v;
        v |= (one_bit << (i & unsigned_bits_log2_mask));
        return x != v;  
    }

    void erase(value_type i)
    {
        m_bit_array[i >> unsigned_bits_log2] &= ~(one_bit << (i & unsigned_bits_log2_mask));
        if (m_iterator_present)
        {
            m_iterator_present = false;
            m_sequence.clear();
        }
    }

    bool test(std::size_t i) const
    {        
        return ((m_bit_array[i >> unsigned_bits_log2] >> (i & unsigned_bits_log2_mask)) & 1) != 0;        
    }

    bool empty() const
    {        
        for (auto x : m_bit_array)
        {
            if (x != 0)
                return false;
        }
        return true;
    }

    void clear()
    {
        if (m_iterator_present)
        {
            m_iterator_present = false;
            m_sequence.clear();
        }
        std::fill(m_bit_array.begin(), m_bit_array.end(), 0);
    }

    std::size_t size() const
    {
        return m_size;
    }

    std::size_t count() const
    {
        if (m_iterator_present)
            return m_sequence.size();

        std::size_t count = 0;
        for (auto x : m_bit_array)
        {
            count += bits::count_bits(x);
        }
        return count;
    }

    iterator begin() const
    {
        if (!m_iterator_present)
        {
            create_iteration_sequence();
            m_iterator_present = true;
        }
        return m_sequence.begin();
    }

    iterator end() const
    {
        if (!m_iterator_present)
        {
            create_iteration_sequence();
            m_iterator_present = true;
        }
        return m_sequence.end();
    }

    reverse_iterator rbegin() const
    {
        if (!m_iterator_present)
        {
            create_iteration_sequence();
            m_iterator_present = true;
        }
        return m_sequence.rbegin();
    }

    reverse_iterator rend() const
    {
        if (!m_iterator_present)
        {            
            create_iteration_sequence();
            m_iterator_present = true;            
        }
        return m_sequence.rend();
    }

    iterator lower_bound(value_type i) const
    {
        return std::lower_bound(m_sequence.begin(), m_sequence.end(), i);
    }

    iterator upper_bound(value_type i) const
    {
        return std::upper_bound(m_sequence.begin(), m_sequence.end(), i);
    }
};

///
/// The unordered sparse set is slower than sparse set, except for iteration over the whole set of values.
/// It uses more memory than sparse set.
///
class unordered_sparse_set
{
public:


    typedef std::vector<std::size_t*> direct_access_sequence;
    typedef std::vector<std::size_t>  iteration_sequence;

    direct_access_sequence m_sparse;
    iteration_sequence m_dense;

public:
    typedef std::size_t value_type;
    typedef std::size_t size_type;
    typedef std::size_t key_type;
    typedef iteration_sequence::const_iterator iterator;
    typedef iterator const_iterator;

    typedef iteration_sequence::const_reverse_iterator reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;

    unordered_sparse_set(std::size_t size) :m_sparse(size), m_dense()
    {
        m_dense.reserve(size);
    }

    unordered_sparse_set() :m_sparse(), m_dense() {}

    void resize(std::size_t size)
    {
        m_sparse.resize(size);
        m_dense.reserve(size);
        m_dense.clear();
    }

    void swap(unordered_sparse_set& s)
    {
        m_sparse.swap(s.m_sparse);
        m_dense.swap(s.m_dense);
    }

    bool test(std::size_t i) const
    {
        return m_sparse[i] != nullptr;
    }

    bool insert(std::size_t i)
    {
        if (m_sparse[i] != nullptr)
            return false;
        m_dense.push_back(i);
        m_sparse[i] = &m_dense.back();
        return true;
    }

    void erase(std::size_t i)
    {
        if (m_sparse[i] == nullptr)
            return;
        std::size_t* v = m_sparse[i];
        std::size_t p_index = m_dense.back();
        if (*v != p_index)
        {
            *v = p_index;
            m_sparse[p_index] = v;
        }
        m_sparse[i] = nullptr;
        m_dense.erase(m_dense.end() - 1);
    }

    void clear()
    {
        std::fill(m_sparse.begin(), m_sparse.end(), nullptr);
        m_dense.clear();
    }

    std::size_t size() const
    {
        return m_sparse.size();
    }

    std::size_t count() const
    {
        return m_dense.size();
    }

    bool empty() const
    {
        return m_dense.empty();
    }

    iterator begin() const
    {
        return m_dense.begin();
    }

    iterator end() const
    {
        return m_dense.end();
    }

    reverse_iterator rbegin() const
    {
        return m_dense.rbegin();
    }

    reverse_iterator rend() const
    {
        return m_dense.rend();
    }

    iterator lower_bound(value_type i) const
    {
        return std::lower_bound(m_dense.begin(), m_dense.end(), i);
    }

    iterator upper_bound(value_type i) const
    {
        return std::upper_bound(m_dense.begin(), m_dense.end(), i);
    }
};

/// 
/// The bounded set is very close to boost::dynamic_bitset
/// It performs practically with the same speed
/// In terms of functinality, it has different member functions (which are similar to those in std::set) and provides an iterator
/// The speed is similar to that of the sparse set, but the repeated iterations over the same set of values are slower.
/// It also uses less memory than sparse set: there is no memory allocation for a vector of values, which is need for the sparse set iterator
///
class bounded_set
{
private:
    static constexpr std::size_t EmptyIndex = static_cast<std::size_t>(-1);
    typedef std::size_t base_type;
    static constexpr unsigned unsigned_bits = std::numeric_limits<base_type>::digits;
    static constexpr unsigned unsigned_bits_log2 = (unsigned_bits == 64 ? 6 : unsigned_bits == 32 ? 5 : unsigned_bits == 16 ? 4 : 3);
    static constexpr unsigned unsigned_bits_log2_mask = 0xFFFFFFFF >> (32 - unsigned_bits_log2);
    static constexpr base_type one_bit = 1;

    unsigned m_size;
    std::vector<base_type> m_bit_array;

public:
    typedef std::size_t value_type;
    typedef std::size_t key_type;
    typedef std::size_t size_type;

    bounded_set(std::size_t size) :m_size(size),
        m_bit_array((m_size + unsigned_bits - 1) / unsigned_bits)
    {
    }

    bounded_set() :m_size(0),
        m_bit_array(0)
    {
    }

    void swap(bounded_set& s)
    {
        m_bit_array.swap(s.m_bit_array);
        std::swap(m_size, s.m_size);
    }

    void resize(std::size_t size)
    {
        m_size = size;
        m_bit_array.resize((m_size + unsigned_bits - 1) / unsigned_bits);
    }

    bool insert(std::size_t i)
    {        
        base_type& v = m_bit_array[i >> unsigned_bits_log2];
        base_type x = v;
        v |= (one_bit << (i & unsigned_bits_log2_mask));
        return x != v;                
    }

    void erase(std::size_t i)
    {
        m_bit_array[i >> unsigned_bits_log2] &= ~(one_bit << (i & unsigned_bits_log2_mask));        
    }

    bool test(std::size_t i) const
    {              
        return ((m_bit_array[i >> unsigned_bits_log2] >> (i & unsigned_bits_log2_mask)) & 1) != 0;
    }

    struct iterator
    {
        friend bounded_set;
        typedef std::forward_iterator_tag
            iterator_category;
        typedef unsigned value_type;
        typedef std::ptrdiff_t
            difference_type;

    private:
        bool test() const
        {
            return ((m_bit_array[m_slot_index] >> m_bit_index) & 1) != 0;
        }        

        bool next()
        {
            m_current_slot >>= 1;

            if (m_current_slot == 0)
            {
                m_bit_index = 0;
                ++m_slot_index;

                while (m_slot_index < m_bit_array_size && (m_current_slot = m_bit_array[m_slot_index]) == 0)
                {
                    ++m_slot_index;
                }

                if (m_current_slot == 0)
                {
                    m_slot_index = EmptyIndex;
                    m_bit_index = 0;
                    m_current_slot = 0;
                    return false;
                }
            }
            else
            {
                ++m_bit_index;
                if (m_slot_index == m_bit_array_size - 1 && m_bit_index > m_last_bit)
                {
                    m_slot_index = EmptyIndex;
                    m_bit_index = 0;
                    m_current_slot = 0;
                    return false;
                }
            }            

            unsigned shift = bits::lsb(m_current_slot);
            m_current_slot >>= shift;
            m_bit_index += shift;

            return true;
        }

        iterator(unsigned size, const std::vector<base_type>&  bit_array, std::size_t pos)
            : m_size(size), m_bit_array_size(bit_array.size()), m_bit_array(size ? &bit_array[0] : nullptr),
            m_slot_index(pos >> unsigned_bits_log2), m_current_slot(0), m_bit_index(pos & unsigned_bits_log2_mask),
            m_last_bit((size - 1) &  unsigned_bits_log2_mask)
        {
            if (pos < m_size)
                m_current_slot = m_bit_array[m_slot_index] >> m_bit_index;

            if (!test())
            {
                next();
            }
        }

    public:

        iterator(unsigned size, const std::vector<base_type>&  bit_array)
            : m_size(size), m_bit_array_size(bit_array.size()), m_bit_array(size ? &bit_array[0] : nullptr), m_slot_index(0), m_current_slot(0), m_bit_index(0),
            m_last_bit((size - 1) &  unsigned_bits_log2_mask)
        {
            if (m_size != 0)
                m_current_slot = m_bit_array[0];

            if (m_size != 0 && !test())
            {
                next();
            }
        }

        iterator() // empty
            : m_size(0), m_bit_array(nullptr), m_slot_index(EmptyIndex), m_current_slot(0), m_bit_index(0), m_last_bit(0)
        {}

        unsigned operator*() const
        {
            return (m_slot_index << unsigned_bits_log2) + m_bit_index;
        }

        iterator& operator++()
        {
            next();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(*this);
            ++*this;
            return tmp;
        }

        bool operator==(const iterator& y)
        {
            return (m_slot_index == y.m_slot_index) && (m_bit_index == y.m_bit_index);
        }

        bool operator!=(const iterator& y)
        {
            return (m_slot_index != y.m_slot_index) || (m_bit_index != y.m_bit_index);
        }

    private:
        unsigned m_size;
        std::size_t m_bit_array_size;
        const base_type* m_bit_array;
        std::size_t m_slot_index;
        base_type m_current_slot;
        unsigned m_bit_index;
        unsigned m_last_bit;
    };

    typedef iterator const_iterator;

    iterator begin() const
    {
        return iterator(m_size, m_bit_array);
    }

    iterator end() const
    {
        return iterator();
    }

    iterator find(unsigned i) const
    {
        if (test(i))
        {
            return iterator(m_size, m_bit_array, i);
        }
        return iterator();
    }

    iterator lower_bound(std::size_t i) const
    {
        return iterator(m_size, m_bit_array, i);        
    }

    iterator upper_bound(std::size_t i) const
    {
        if (i + 1 >= m_size)
            return iterator();        
        return iterator(m_size, m_bit_array, i+1);
    }

    void erase(const iterator& it)
    {
        erase(*it);
    }

    bool empty() const
    {
        for (auto x : m_bit_array)
        {
            if (x != 0)
                return false;
        }
        return true;
    }

    void clear()
    {
        std::fill(m_bit_array.begin(), m_bit_array.end(), 0);
    }

    std::size_t size() const
    {
        return m_size;
    }

    std::size_t count() const
    {
        std::size_t count = 0;
        for (auto x : m_bit_array)
        {
            count += bits::count_bits(x);
        }
        return count;
    }
}; // bounded set
