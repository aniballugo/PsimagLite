#ifndef BIT_MANIP_H
#define BIT_MANIP_H

namespace PsimagLite {
	namespace BitManip {
		//Counting bits set, Brian Kernighan's way
		typedef unsigned int long long WordType;
		inline size_t countKernighan(WordType v)
		{
			size_t c = 0; // c accumulates the total bits set in v
			for (; v; c++) v &= v - 1; // clear the least significant bit set
			return c;
		}

		inline int count (WordType b)
		{
#if (ULONG_MAX == 0xfffffffful)
			b = (0x55555555 & b) + (0x55555555 & (b >> 1));
			b = (0x33333333 & b) + (0x33333333 & (b >> 2));
			b = (0x0f0f0f0f & b) + (0x0f0f0f0f & (b >> 4));
			b = (0x00ff00ff & b) + (0x00ff00ff & (b >> 8));
			b = (0x0000ffff & b) + (0x0000ffff & (b >> 16));

			return (int) b;
#else
			b = (0x5555555555555555 & b) + (0x5555555555555555 & (b >> 1));
			b = (0x3333333333333333 & b) + (0x3333333333333333 & (b >> 2));
			b = (0x0f0f0f0f0f0f0f0f & b) + (0x0f0f0f0f0f0f0f0f & (b >> 4));
			b = (0x00ff00ff00ff00ff & b) + (0x00ff00ff00ff00ff & (b >> 8));
			b = (0x0000ffff0000ffff & b) + (0x0000ffff0000ffff & (b >> 16));
			b = (0x00000000ffffffff & b) + (0x00000000ffffffff & (b >> 32));

			return (int) b;
#endif
		}
	} // namespace BitManip
} // namespace PsimagLite


#endif // BIT_MANIP_H

