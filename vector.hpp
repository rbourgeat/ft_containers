/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/16 18:45:56 by rbourgea          #+#    #+#             */
/*   Updated: 2021/09/15 12:56:44 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_HPP
# define VECTOR_HPP

# include <memory>
# include <stdexcept>
# include <iostream>
# include <cstring>
# include "common/Others.hpp"
# include "common/Random_access_iterator.hpp"
# include "common/Reverse_iterator.hpp"
# include "common/Lexicographical_compare.hpp"
# include "common/Iterator.hpp"
# include "common/Enable_if.hpp"
# include "common/Is_integral.hpp"

// For your vector implementation, it is not mandatory to code the vector<bool> specialization.

namespace ft
{
	template <class T, class Alloc = std::allocator<T> >
	class vector
	{
		public:
			typedef T												value_type;
			typedef Alloc											allocator_type;
			typedef typename allocator_type::reference				reference;
			typedef typename allocator_type::const_reference		const_reference;
			typedef typename allocator_type::pointer				pointer;
			typedef typename allocator_type::const_pointer			const_pointer;
			typedef ft::random_access_iterator<value_type>			iterator;
			typedef ft::random_access_iterator<const value_type>	const_iterator;
			typedef ft::reverse_iterator<iterator>					reverse_iterator;
    		typedef	ft::reverse_iterator<const_iterator>			const_reverse_iterator;
			typedef typename ft::iterator_traits<iterator>::difference_type	difference_type;
			typedef typename allocator_type::size_type				size_type;

			// Constructors:
			// Constructs a vector, initializing its contents depending on the constructor version used:
			// (1) empty container constructor (default constructor)
			explicit vector(const allocator_type &alloc = allocator_type()) : // Construct an empty container
				_alloc(alloc), _start(u_nullptr), _end(u_nullptr), _end_capacity(u_nullptr)
			{
				
			}
			// (2) fill constructor
			explicit vector (size_type n, const value_type& val = value_type(), const allocator_type& alloc = allocator_type()) :
				_alloc(alloc), _start(u_nullptr), _end(u_nullptr), _end_capacity(u_nullptr) // Constructs a container with "n" elements
			{
				_start = _alloc.allocate(n);
				_end_capacity = _start + n;
				_end = _start;
				while (n--)
				{
					_alloc.construct(_end, val);
					_end++;
				}
			}
			// (3) range constructor
			template <class InputIterator> 
			vector (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type(), 
				typename ft::enable_if<!ft::is_integral<InputIterator>::value, InputIterator>::type* = u_nullptr) : _alloc(alloc)
			{
				bool is_valid;
				if (!(is_valid = ft::is_ft_iterator_tagged<typename ft::iterator_traits<InputIterator>::iterator_category >::value))
					throw (ft::InvalidIteratorException<typename ft::is_ft_iterator_tagged<typename ft::iterator_traits<InputIterator>::iterator_category >::type>());

				difference_type n = ft::distance(first, last);
				_start = _alloc.allocate(n);
				_end_capacity = _start + n;
				_end = _start;
				while (n--)
				{
					_alloc.construct(_end, *first++);
					_end++;
				}
			}
			// (4) copy constructor
			vector (const vector& x) : _alloc(x._alloc), _start(u_nullptr), _end(u_nullptr), _end_capacity(u_nullptr)
			{
				insert(begin(), x.begin(), x.end());
			}

			~vector() // Destructor
			{
				clear();
				_alloc.deallocate(_start, capacity());
			}

			vector &operator=(const vector &x) // Assign content
			{
				if (x == *this)
					return (*this);
				clear();
				insert(end(), x.begin(), x.end());
				return (*this);
			}

			// Iterators:
			iterator begin()
			{
				return (_start);
			}
			const_iterator begin() const
			{
				return (_start);
			}
			iterator end()
			{
				if (empty())
					return (begin());
				return (_end);
			}
			const_iterator end() const
			{
				if (empty())
					return (begin());
				return (_end);
			}
			reverse_iterator rbegin()
			{
				return (reverse_iterator(end()));
			}
			const_reverse_iterator rbegin() const
			{
				return (reverse_iterator(end()));
			}
			reverse_iterator rend()
			{
				return (reverse_iterator(begin()));
			}
			const_reverse_iterator rend() const
			{
				return (reverse_iterator(begin()));
			}

			// Capacity:
			size_type size() const // Return size
			{
				return (_end - _start);
			}
			size_type max_size() const // Return maximum size
			{
				return allocator_type().max_size();
			}
			void resize(size_type n, value_type val = value_type()) // Change size
			{
				if (n > max_size())
					throw (std::length_error("vector::resize"));
				else if (n < size())
				{
					while (size() > n)
					{
						--_end;
						_alloc.destroy(_end);
					}
				}
				else
					insert(end(), n - size(), val);
			}
			size_type capacity() const // Return size of allocated storage capacity
			{
				return (_end_capacity - _start);
			}
			bool empty() const // Test whether vector is empty
			{
				return (size() == 0 ? true : false);
			}
			void reserve(size_type n) // Request a change in capacity
			{
				if (n > max_size())
					throw(std::length_error("vector::reserve"));
				else if (n > capacity())
				{
					pointer prev_start = _start;
					pointer prev_end = _end;
					size_type prev_size = size();
					size_type prev_capacity = capacity();

					_start = _alloc.allocate(n);
					_end_capacity = _start + n;
					_end = _start;
					while (prev_start != prev_end)
					{
						_alloc.construct(_end, *prev_start);
						_end++;
						prev_start++;
					}
					_alloc.deallocate(prev_start - prev_size, prev_capacity);
				}
			}

			// Element access:
			reference operator[](size_type n) // Access element
			{
				return (*(_start + n));
			}
			const_reference operator[](size_type n) const
			{
				return (*(_start + n));
			}
			reference at(size_type n) // Access element
			{
				checkRange(n);
				return ((*this)[n]);
			}
			const_reference at(size_type n) const
			{
				if (n >= this->_size)
				{
					throw std::out_of_range("ft::vector::at");
				}
				checkRange(n);
				return ((*this)[n]);
			}
			reference front() // Access first element
			{
				return (*_start);
			}
			const_reference front() const
			{
				return (*_start);
			}
			reference back() // Access last element
			{
				return (*(_end - 1));
			}
			const_reference back() const
			{
				return (*(_end - 1));
			}

			// Modifiers:

			// std::vector::assign
			// range (1) :
			// In the range version (1), the new contents are elements constructed from each of the elements in the range between first and last, in the same order.
			template <class InputIterator>
			void assign(InputIterator first, InputIterator last, typename ft::enable_if<!ft::is_integral<InputIterator>::value, InputIterator>::type* = u_nullptr)
			{
				bool is_valid;
				if (!(is_valid = ft::is_ft_iterator_tagged<typename ft::iterator_traits<InputIterator>::iterator_category>::value))
					throw(ft::InvalidIteratorException<typename ft::is_ft_iterator_tagged<typename ft::iterator_traits<InputIterator>::iterator_category>::type>());
				clear();
				size_type dist = ft::distance(first, last);
				if (size_type(_end_capacity - _start) >= dist)
				{
					for (; &(*first) != &(*last); first++, _end++)
						_alloc.construct(_end, *first);
				}
				else
				{
					pointer new_start = pointer();
					pointer new_end = pointer();
					pointer new_end_capacity = pointer();

					new_start = _alloc.allocate(dist);
					new_end = new_start;
					new_end_capacity = new_start + dist;

					for (; &(*first) != &(*last); first++, new_end++)
						_alloc.construct(new_end, *first);

					_alloc.deallocate(_start, capacity());

					_start = new_start;
					_end = new_end;
					_end_capacity = new_end_capacity;
				}
			}
			// std::vector::assign
			// fill (2) :
			// In the fill version (2), the new contents are n elements, each initialized to a copy of val.
			void assign(size_type n, const value_type &val)
			{
				clear();
				if (n == 0)
					return;
				if (size_type(_end_capacity - _start) >= n)
				{
					while (n)
					{
						_alloc.construct(_end++, val);
						n--;
					}
				}
				else
				{
					_alloc.deallocate(_start, capacity());
					_start = _alloc.allocate(n);
					_end = _start;
					_end_capacity = _start + n;
					while (n)
					{
						_alloc.construct(_end++, val);
						n--;
					}
				}
			}
			void push_back(const value_type &val) // Add element at the end
			{
				if (_end == _end_capacity)
				{
					int next_capacity = (size() > 0) ? (int)(size() * 2) : 1;
					reserve(next_capacity);
				}
				_alloc.construct(_end, val);
				_end++;
			}
			void pop_back() // Delete last element
			{
				_alloc.destroy(&back());
				_end--;
			}
			// single element (1)
			iterator insert(iterator position, const value_type &val)
			{
				size_type pos_len = &(*position) - _start;
				if (size_type(_end_capacity - _end) >= this->size() + 1)
				{
					for (size_type i = 0; i < pos_len; i++)
						_alloc.construct(_end - i, *(_end - i - 1));
					_end++;
					_alloc.construct(&(*position), val);
				}
				else
				{
					pointer new_start = pointer();
					pointer new_end = pointer();
					pointer new_end_capacity = pointer();

					int next_capacity = (this->size() * 2 > 0) ? this->size() * 2 : 1;
					new_start = _alloc.allocate(next_capacity);
					new_end = new_start + this->size() + 1;
					new_end_capacity = new_start + next_capacity;

					for (size_type i = 0; i < pos_len; i++)
						_alloc.construct(new_start + i, *(_start + i));
					_alloc.construct(new_start + pos_len, val);
					for (size_type j = 0; j < this->size() - pos_len; j++)
						_alloc.construct(new_end - j - 1, *(_end - j - 1));

					for (size_type l = 0; l < this->size(); l++)
						_alloc.destroy(_start + l);
					if (_start)
						_alloc.deallocate(_start, this->capacity());

					_start = new_start;
					_end = new_end;
					_end_capacity = new_end_capacity;
				}
				return (iterator(_start + pos_len));
			}
			// fill (2)
			void insert(iterator position, size_type n, const value_type &val)
			{
				if (n == 0)
					return;
				if (n > this->max_size())
					throw(std::length_error("vector::insert (fill)"));
				size_type pos_len = &(*position) - _start;
				if (size_type(_end_capacity - _end) >= n)
				{
					for (size_type i = 0; i < this->size() - pos_len; i++)
						_alloc.construct(_end - i + (n - 1), *(_end - i - 1));
					_end += n;
					while (n)
					{
						_alloc.construct(&(*position) + (n - 1), val);
						n--;
					}
				}
				else
				{
					pointer new_start = pointer();
					pointer new_end = pointer();
					pointer new_end_capacity = pointer();

					int next_capacity = (this->capacity() > 0) ? (int)(this->size() * 2) : 1;
					new_start = _alloc.allocate(next_capacity);
					new_end_capacity = new_start + next_capacity;

					if (size_type(new_end_capacity - new_start) < this->size() + n)
					{
						if (new_start)
							_alloc.deallocate(new_start, new_start - new_end_capacity);
						next_capacity = this->size() + n;
						new_start = _alloc.allocate(next_capacity);
						new_end = new_start + this->size() + n;
						new_end_capacity = new_start + next_capacity;
					}

					new_end = new_start + this->size() + n;

					for (int i = 0; i < (&(*position) - _start); i++)
						_alloc.construct(new_start + i, *(_start + i));
					for (size_type k = 0; k < n; k++)
						_alloc.construct(new_start + pos_len + k, val);
					for (size_type j = 0; j < (this->size() - pos_len); j++)
						_alloc.construct(new_end - j - 1, *(_end - j - 1));

					for (size_type u = 0; u < this->size(); u++)
						_alloc.destroy(_start + u);
					_alloc.deallocate(_start, this->capacity());

					_start = new_start;
					_end = new_end;
					_end_capacity = new_end_capacity;
				}
			}
			// range (3)
			template <typename InputIterator>
			void insert(iterator position, InputIterator first, InputIterator last,
				typename ft::enable_if<!ft::is_integral<InputIterator>::value, InputIterator>::type* = u_nullptr)
			{
				bool is_valid;
				if (!(is_valid = ft::is_ft_iterator_tagged<typename ft::iterator_traits<InputIterator>::iterator_category>::value))
					throw (ft::InvalidIteratorException<typename ft::is_ft_iterator_tagged<typename ft::iterator_traits<InputIterator>::iterator_category>::type>());
				
				size_type dist = ft::distance(first, last);
				if (size_type(_end_capacity - _end) >= dist)
				{
					for (size_type i = 0; i < size() - (&(*position) - _start); i++)
						_alloc.construct(_end - i + (dist - 1), *(_end - i - 1));
					_end += dist;
					for (; &(*first) != &(*last); first++, position++)
						_alloc.construct(&(*position), *first);
				}
				else
				{
					pointer new_start = pointer();
					pointer new_end = pointer();
					pointer new_end_capacity = pointer();

					new_start = _alloc.allocate(size() * 2);
					new_end = new_start + size() + dist;
					new_end_capacity = new_start + (size() * 2 );

					if (size_type(new_end_capacity - new_start) < size() + dist)
					{
						if (new_start)
							_alloc.deallocate(new_start, new_end_capacity - new_start);
						new_start = _alloc.allocate (size() + dist);
						new_end = new_start + size() + dist;
						new_end_capacity = new_end;
					}

					for (int i = 0; i < &(*position) - _start; i++)
						_alloc.construct(new_start + i, *(_start + i));
					for (int j = 0; &(*first) != &(*last); first++, j++)
						_alloc.construct(new_start + (&(*position) - _start) + j, *first);
					for (size_type k = 0; k < size() - (&(*position) - _start); k++)
						_alloc.construct(new_start + (&(*position) - _start) + dist + k, *(_start + (&(*position) - _start) + k));

					for (size_type l = 0; l < size(); l++)
						_alloc.destroy(_start + l);
					_alloc.deallocate(_start, capacity());

					_start = new_start;
					_end = new_end;
					_end_capacity = new_end_capacity;
				}
				
			}
			iterator erase(iterator position)
			{
				pointer p_pos = &(*position);
				_alloc.destroy(&(*position));
				if (&(*position) + 1 == _end)
					_alloc.destroy(&(*position));
				else
				{
					for (int i = 0; i < _end - &(*position) - 1; i++)
					{
						_alloc.construct(&(*position) + i, *(&(*position) + i + 1));
						_alloc.destroy(&(*position) + i + 1);
					}
				}
				_end -= 1;
				return (iterator(p_pos));
			}
			iterator erase(iterator first, iterator last)
			{
				pointer p_first = &(*first);
				for (; &(*first) != &(*last); first++)
					_alloc.destroy(&(*first));
				for (int i = 0; i < _end - &(*last); i++)
				{
					_alloc.construct(p_first + i, *(&(*last) + i));
					_alloc.destroy(&(*last) + i);
				}
				_end -= (&(*last) - p_first);
				return (iterator(p_first));
			}
			void swap(vector &x)
			{
				if (x == *this)
					return;

				pointer save_start = x._start;
				pointer save_end = x._end;
				pointer save_end_capacity = x._end_capacity;
				allocator_type save_alloc = x._alloc;

				x._start = _start;
				x._end = _end;
				x._end_capacity = _end_capacity;
				x._alloc = _alloc;

				_start = save_start;
				_end = save_end;
				_end_capacity = save_end_capacity;
				_alloc = save_alloc;
			}
			void clear()
			{
				size_type save_size = size();
				for (size_type i = 0; i < save_size; i++)
				{
					_end--;
					_alloc.destroy(_end);
				}
			}

		private:
			allocator_type _alloc;
			pointer _start;
			pointer _end;
			pointer _end_capacity;

			void checkRange(const size_type& n) const
			{
				if (n >= this->size())
					throw (std::out_of_range("vector::checkRange: n (which is "
							+ ft::to_string(n) + ") >= this->size() (which is "
							+ ft::to_string(this->size()) + ")"));
			}
	};

	// Non-member function overloads:
	// Relational operators:
	template <class T, class Alloc>
	bool operator== (const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
	{
		if (lhs.size() != rhs.size())
			return (false);
		typename ft::vector<T>::const_iterator first1 = lhs.begin();
		typename ft::vector<T>::const_iterator first2 = rhs.begin();
		while (first1 != lhs.end())
		{
			if (first2 == rhs.end() || *first1 != *first2)
				return (false);
			++first1;
			++first2;
		}
		return (true);
	}

	template <class T, class Alloc>
	bool operator!= (const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
	{
		return (!(lhs == rhs));
	}

	template <class T, class Alloc>
	bool operator< (const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
	{
		return (ft::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
	}

	template <class T, class Alloc>
	bool operator<= (const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		return (!(lhs > rhs));
	}

	template <class T, class Alloc>
	bool operator>(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
	{
		return (rhs < lhs);
	}

	template <class T, class Alloc>
	bool operator>= (const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
	{
		return (!(lhs < rhs));
	}

	template <class T, class Alloc>
	void swap (vector<T,Alloc>& x, vector<T,Alloc>&y)
	{
		x.swap(y);
	}
}

#endif