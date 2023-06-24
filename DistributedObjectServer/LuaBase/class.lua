--class.lua
if not _class then
    _class = { }
end

function class(super)
    -- 一个类的构建，这里构建的是类本身，ctor是构造函数，super是父类，这里继承只允许一个父类
    local class_type = { ctor = false, super = super }
    --vtbl是当前类中所有域存放的地方
    local vtbl = { }
    _class[class_type] = vtbl
    --_class[super]这里返回的是super 本身作为class_type 对应的vtbl
    --父类的vtbl
    vtbl.super = _class[super]  
    class_type.superclass = _class[super]

    --设置class_type类本身的元方法，这里操作的是vtbl，并没有修改class_type本身（查找域和添加域都是操作的vtbl，class_type只是简单的原型）
    setmetatable(class_type, {
        __newindex = function(t, k, v) vtbl[k] = v end,
        __index = function(t, k) return vtbl[k] end,
    } )

    if super then
        --关联父类子类的关系的查找域，vtbl关联父类的btbl查找域
        setmetatable(vtbl, {
            __index =
            function(t, k)
                if k and _class[super] then
                    local ret = _class[super][k]
                    vtbl[k] = ret
                    return ret
                else
                    return nil
                end
            end
        } )
    end

    class_type.New = function(...)
        -- 一个类实例的构建
        local obj = { class = class_type }
        -- 设置实例关联类的查找域vtbl
        setmetatable(obj, {
            __index =
            function(t, k)
                return _class[class_type][k]
            end
        } )

        --类和所有父类的ctor构造函数收集，第一个当前类的ctor，第二个父类的ctor，第三个父类的父类的ctor，....
        local inherit_list = { }
        local class_ptr = class_type
        while class_ptr do
            if class_ptr.ctor then table.insert(inherit_list, class_ptr) end
            class_ptr = class_ptr.super
        end
        local inherit_length = #inherit_list
        --调用所有构造函数，从最上层的父类ctor开始知道当前类的ctor
        if inherit_length > 0 then
            for i = inherit_length, 1, -1 do 
                inherit_list[i].ctor(obj, ...)
            end
        end
        obj.super = inherit_list[2];

--        if detectMemoryLeak then
--            registerToWeakTable(obj, debug.traceback("obj lua stack:"));
--        end
--
        obj.class = class_type

        return obj
    end

    class_type.is = function(self_ptr, compare_class)
        if not compare_class or not self_ptr then return false end
        local raw_class = self_ptr.class
        while raw_class do
            if raw_class == compare_class then return true end
            raw_class = raw_class.super
        end
        return false
    end

    return class_type
end

