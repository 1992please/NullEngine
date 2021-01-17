#pragma once

/**
 * Used to declare an untyped array of data with compile-time alignment.
 */
template<int32 Size, uint32 Alignment>
struct TAlignedBytes; // this intentionally won't compile, we don't support the requested alignment

/** Unaligned storage. */
template<int32 Size>
struct TAlignedBytes<Size, 1>
{
	uint8 Pad[Size];
};

#define IMPLEMENT_ALIGNED_STORAGE(Align) \
	template<int32 Size>        \
	struct TAlignedBytes<Size,Align> \
	{ \
		struct alignas(Align) TPadding \
		{ \
			uint8 Pad[Size]; \
		}; \
		TPadding Padding; \
	};

// Implement TAlignedBytes for these alignments.
IMPLEMENT_ALIGNED_STORAGE(16);
IMPLEMENT_ALIGNED_STORAGE(8);
IMPLEMENT_ALIGNED_STORAGE(4);
IMPLEMENT_ALIGNED_STORAGE(2);

#undef IMPLEMENT_ALIGNED_STORAGE

/** An untyped array of data with compile-time alignment and size derived from another type. */
template<typename ElementType>
struct TTypeCompatibleBytes :
	public TAlignedBytes<
	sizeof(ElementType),
	alignof(ElementType)
	>
{
	ElementType*		GetTypedPtr() { return (ElementType*)this; }
	const ElementType*	GetTypedPtr() const { return (const ElementType*)this; }
};