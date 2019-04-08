#pragma once

enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{    
    static constexpr char Separator = ' ';
    std::ostream& out_;

public:
    explicit Serializer(std::ostream& out)
        : out_(out)
    {
    }

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
        return process(std::forward<ArgsT>(args)...);
    }

private:
    template <class T, class... ArgsT>
    Error process(T&& value, ArgsT&& ... args)
    {
        process(std::forward<T>(value));
        return process(std::forward<ArgsT>(args)...);
    }

    template <class T>
    Error process(T&& val)
    {
        out_ << std::boolalpha << val << Separator;
        return Error::NoError;
    }

};

class Deserializer
{
    std::istream& in_;

public:
    explicit Deserializer(std::istream& in): in_(in)
    {
    };

    template<class... ArgsT>
    Error operator()(ArgsT&& ... args)
    {
        return process(std::forward<ArgsT>(args)...);
    }
    
    template<class T>
    Error load(T& object)
    {
        return object.serialize(*this);
    }

private:
    template<class T, class... ArgsT>
    Error process(T&& value, ArgsT&& ... args)
    {
        if (process(std::forward<T>(value)) == Error::NoError)
            return process(std::forward<ArgsT>(args)...);
        else 
            return Error::CorruptedArchive;
    }

    template <class T>
    Error process(T&& val)
    {
        std::string text;
        in_ >> text;
        if (std::is_same_v<std::remove_reference_t<T>, bool>) 
        {
            if (text == "true")
                val = true;
            else if (text == "false")
                val = false;
            else
                return Error::CorruptedArchive;
        }
        else if (isdigit(text[0]))
        {
            int k = stoi(text);
            if (k < 0)
                return Error::CorruptedArchive;
            else 
            {
                val = k;
                return Error::NoError;
            }
        }
        else
            return Error::CorruptedArchive;
        return Error::NoError;
    }
};