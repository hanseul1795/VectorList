#include <vector>
#include <list>
#include <string>
#include <cstddef>
#include <stdio.h>
#include <algorithm>
#include "basic_string.h"
#include "Vector.h"
#include "List.h"


#define USE_MOVE 	0
#define Vector		vector
#define List 		List
#define String 		string<char, std::char_traits<char>, SpyAllocator<char> >

class CMemorySpy
{
	public:
		void	NotifyAlloc(size_t size, size_t count, void* address)
		{
			printf("*** Allocate block %d : %ld elem of %ld bytes\n", s_curId, count, size);
			m_blocks.push_back({ size, count, address, s_curId++ });
		}

		void	NotifyDealloc(void* address, size_t count)
		{
			for (size_t i = 0; i < m_blocks.size(); ++i)
			{
				if (m_blocks[i].address == address)
				{
					if (m_blocks[i].count == count)
					{
						printf("*** Deallocate block %d : %ld elem of %ld bytes\n", m_blocks[i].id, m_blocks[i].count, m_blocks[i].size);
					}
					else
					{
						printf("*** Trying to deallocate block %d of %ld elem while %ld where allocated !\n", m_blocks[i].id, count, m_blocks[i].count);
					}
					if (i + 1 < m_blocks.size())
					{
						m_blocks[i] = m_blocks[m_blocks.size() - 1];
					}
					m_blocks.resize(m_blocks.size() - 1);
					return;
				}
			}

			printf("*** Deallocate failed !\n");
		}

		void	CheckLeaks()
		{
			if (m_blocks.empty())
			{
				printf("*** No memory leak\n");
			}
			else
			{
				printf("*** %ld memory leaks detected !\n", m_blocks.size());
				for (const MemBlock& block : m_blocks)
				{
					printf("- Leak of block %d : %ld elem of size %ld \n", block.id, block.count, block.size);
				}
			}
		}

		static int	s_curId;

	private:
		struct MemBlock
		{
			size_t 	size;
			size_t 	count;
			void* 	address;
			int 	id;
		};

		std::vector<MemBlock>	m_blocks;
};

int 				CMemorySpy::s_curId = 0;
static CMemorySpy	memorySpy;

template <class T>
struct SpyAllocator : std::allocator<T>
{
	typedef T	value_type;

	SpyAllocator(/*vector args*/) = default;

	template<class U>
	SpyAllocator(const SpyAllocator<U>& other){}

	template<class U>
	struct rebind
	{
		using other = SpyAllocator<U>;
	};

	T*	allocate(std::size_t n)
	{
		T* p = (T*) new char[sizeof(T) * n];
		
		memorySpy.NotifyAlloc(sizeof(T), n, p);
		return p;
	};
	
	void	deallocate(T* p, std::size_t n)
	{
		memorySpy.NotifyDealloc(p, n);
		delete (char*)p;
	}

	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
};

template <class T, class U>
bool	operator==(const SpyAllocator<T>&, const SpyAllocator<U>&) { return false; }

template <class T, class U>
bool	operator!=(const SpyAllocator<T>&, const SpyAllocator<U>&) { return false; }

class	Foo
{
	public:

		Foo()
		{
			m_id = count++;
			printf("Create Foo_%d\n", m_id);
		}

		Foo(const Foo& foo)
		{
			m_id = count++;
			printf("Create Foo_%d copy of Foo_%d\n", m_id, foo.m_id);
		}

		Foo& operator=(const Foo& foo)
		{
			printf("Copying Foo_%d into Foo_%d\n", foo.m_id, m_id);
			return *this;
		}

#if USE_MOVE
		Foo& operator=(Foo&& foo)
		{
			printf("Moving Foo_%d into Foo_%d\n", foo.m_id, m_id);
			return *this;
		}

		Foo(Foo&& foo)
		{
			m_id = count++;
			printf("Create Foo_%d move copy of Foo_%d\n", m_id, foo.m_id);
		}
#endif

		virtual ~Foo()
		{
			printf("Destroy Foo_%d\n", m_id);
		}

		static void ResetCount()
		{
			count = 0;
		}

	private:
		int	m_id;

		static int count;
};

int	Foo::count;

#define DO(inst) printf("\n" #inst "\n"); inst

template<typename T, class Allocator>
class	MonContainer
{
	public:
		MonContainer()
		{
			// Create array of 5 elements
			m_elements = Allocator().allocate(5); // allocate data
			for (int i = 0; i < 5; ++i)
			{
				new (&m_elements[i]) T; // construct each element
			}
		}

		~MonContainer()
		{
			for (int i = 0; i < 5; ++i)
			{
				m_elements[i].~T(); // destroy each element
			}

			// Destroy the 5 elements
			Allocator().deallocate(m_elements, 5);
		}

	private:
		T*	m_elements;
};

int main()
{
	{
		printf("===========Test container============\n");
		Foo::ResetCount();
		printf("Create container\n");

		MonContainer<Foo, SpyAllocator<Foo>>	foos;

		printf("Destroy container\n");
	}

	Foo::ResetCount();

	{
		printf("=======Vector of int================\n\nCreate vector\n");
		Vector<int, SpyAllocator<int>>	intVec;

		printf("\nsizeof(intVec) : %ld\n", sizeof(intVec));
		printf("\nCapacity=%ld, Size=%ld\n", intVec.capacity(), intVec.size());

		DO(intVec.push_back(10);)
		DO(intVec.push_back(20);)
		DO(intVec.push_back(30);)
		DO(intVec.push_back(40);)
		DO(intVec.push_back(50);)

		printf("\nCapacity=%ld, Size=%ld\n", intVec.capacity(), intVec.size());

		DO(intVec[3] *= 2;)

 		for (size_t i = 0; i < intVec.size(); ++i)
 		{
 			printf("%d ", intVec[i]);
 		}
 		printf("\n");

		printf("\nDestroy vector\n\n");
	}


	{
		printf("\n=======Vector of Foo================\n\nCreate vector\n");

		Vector<Foo, SpyAllocator<Foo> > fooVec;
	
		printf("\nsizeof(fooVec) : %ld\n", sizeof(fooVec));	
		
		DO(fooVec.reserve(2));
		DO(fooVec.push_back(Foo()));
		DO(fooVec.push_back(Foo()));
		DO(fooVec.push_back(Foo()));

		printf("\nDestroy vector\n\n");
	}

	{
		printf("\n=======Vector of Foo================\n\nCreate vector\n");

		Vector<Foo, SpyAllocator<Foo> > fooVec;

		DO(fooVec.reserve(2));

		DO(fooVec.push_back(Foo()));
		DO(fooVec.push_back(Foo()));
		DO(fooVec.clear());

		DO(fooVec.push_back(Foo()));

		printf("\nDestroy vector\n\n");
	}


	{
		printf("\n=======Vector of Foo================\n\nCreate vector\n");

		Vector<Foo, SpyAllocator<Foo> > fooVec;

		DO(fooVec.resize(2));

		DO(fooVec.push_back(Foo()));
		DO(fooVec.push_back(Foo()));
		
		printf("\nDestroy vector\n\n");
	}

	{
		printf("\n=======Vector of Foo================\n\nCreate vector\n");

		Vector<Foo, SpyAllocator<Foo> > fooVec;

		DO(fooVec.push_back(Foo()));
		DO(fooVec.push_back(Foo()));

		printf("\nCopy fooVec\n\n");
	
		Vector<Foo, SpyAllocator<Foo> > fooVec2 = fooVec;
	
		printf("\nDestroy vectors \n");
	}
	

	{
		printf("\n=======Vector of Foo================\n\nCreate vector\n");

		Vector<Foo, SpyAllocator<Foo> > fooVec;

		DO(fooVec.push_back(Foo()));
		DO(fooVec.push_back(Foo()));

		printf("\nCopy fooVec\n\n");
	
		Vector<Foo, SpyAllocator<Foo> > fooVec2;
		fooVec2 = fooVec;
	
		printf("\nDestroy vectors \n");
	}
	
	
	{
		printf("\n=======List================\n\nCreate list\n");

		List<int, SpyAllocator<int>> list;
		
		DO(list.push_back(10));	
		DO(list.push_back(20));
		DO(list.push_back(30));
		DO(list.push_back(40));
		DO(list.push_back(50));

		printf("\nList size : %ld\n", list.size());


		printf("\nPrint list:\n");
		for (List<int, SpyAllocator<int>>::iterator it = list.begin(); it != list.end(); ++it)
		{
			printf("%d ", *it);
		}
		printf("\n\n");


		DO(list.remove(20));
		DO(list.remove(40));

		for (List<int, SpyAllocator<int>>::iterator it = list.begin(); it != list.end(); ++it)
		{
			printf("%d ", *it);
		}
		printf("\n\n");


		DO(list.insert(std::find(list.begin(), list.end(), 30), 60));

		printf("\nList size : %ld\n", list.size());

		printf("\nPrint list:\n");
		for (List<int, SpyAllocator<int>>::iterator it = list.begin(); it != list.end(); ++it)
		{
			printf("%d ", *it);
		}
		printf("\n\n");

		printf("\nDestroy list\n\n");
	}


	{
		printf("\n=======List================\n\nCreate list\n");

		std::list<int, SpyAllocator<int>>	list;
		
		DO(list.push_back(10));	
		DO(list.push_back(20));
		DO(list.push_back(30));
		DO(list.push_back(40));
		DO(list.push_back(50));


		printf("\n\nCopy list = list2\n");
		std::list<int, SpyAllocator<int>>	list2 = list;

		DO(list.remove(20));
		DO(list.remove(40));
		DO(list.insert(std::find(list.begin(), list.end(), 30), 60));



		printf("\nPrint list:\n");
		for (std::list<int, SpyAllocator<int>>::iterator it = list.begin(); it != list.end(); ++it)
		{
			printf("%d ", *it);
		}
		printf("\n\n");

		printf("\nPrint list2:\n");
		for (std::list<int, SpyAllocator<int>>::iterator it = list2.begin(); it != list2.end(); ++it)
		{
			printf("%d ", *it);
		}
		printf("\n\n");

		printf("\nDestroy lists\n\n");
	}

	{
		printf("\n=======Strings================\n");

		printf("Sizeof String : %ld\n", sizeof(String));

		{
			DO(String s);
			printf("Length of s : %ld\n", s.length());
		}

		{
			DO(String s = "Moins de 16 car");
			printf("Length of s : %ld\n", s.length());
			s.end();
		}
		
		{
			DO(String s = "Plus de 16 car, 27 au total");
			printf("Length of s : %ld\n", s.length());
		}

		{
			DO(String s = "Plus de 32 car, 34 pour etre exact");
			printf("Length of s : %ld\n", s.length());
		}

		{
			DO(String s = "Plus de 48 car, 51 pour etre exact, bon ca suffit ?");
			printf("Length of s : %ld\n", s.length());
		}

		{
			DO(String s1 = "toto est vraiment une personne incroyable");
			DO(String s2 = s1);
			DO(s1[1] = 'a');
			DO(s1[3] = 'a');
			printf("s1 : %s\n", s1.c_str());
			printf("s2 : %s\n", s2.c_str());
			DO(s2 = s1);
			DO(s2[3] = 'i');
			printf("s1 : %s\n", s1.c_str());
			printf("s2 : %s\n", s2.c_str());
		}
			
		DO(String s1 = "Bonjour");
		std::cout<<s1.at(3)<<std::endl;
		DO(String s2 = " le monde !");
		DO(String s3 = s1 + s2);
		printf("s3 : %s\n", s3.c_str());


		printf("\nDestroy vector\n\n");
	}

	memorySpy.CheckLeaks();

	system("pause");

    return 0;
}
