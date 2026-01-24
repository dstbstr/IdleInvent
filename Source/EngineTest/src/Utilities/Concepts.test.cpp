#include "Utilities/Concepts.h"

enum struct Thing {A, B};
struct TypeWithId {
	using IdType = Thing;
	Thing Id{Thing::A};
};
static_assert(HasId<TypeWithId>);

struct TypeWithoutIdAlias {
    Thing Id{Thing::A};
};
static_assert(!HasId<TypeWithoutIdAlias>);

struct TypeWithWrongTypeId {
	using IdType = int;
	Thing Id{Thing::A};
};
static_assert(!HasId<TypeWithWrongTypeId>);