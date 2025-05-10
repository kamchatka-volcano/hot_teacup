#ifndef HOT_TEACUP_VIEW_OR_OWNER_INTERFACE_H
#define HOT_TEACUP_VIEW_OR_OWNER_INTERFACE_H

namespace http::detail {

class IViewOrOwner {
protected:
    virtual ~IViewOrOwner() = default;

public:
    virtual bool isView() const = 0;
    virtual void makeOwnStateFromView() = 0;
};

}

#endif //HOT_TEACUP_VIEW_OR_OWNER_INTERFACE_H