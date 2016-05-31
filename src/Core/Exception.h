/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <exception>
#include <string>

namespace PyMesh {

class PyMeshException : public std::exception {
    public:
        PyMeshException(const std::string& description) :
            exception(), m_description(description) {}
        virtual ~PyMeshException() throw() {}

    public:
        virtual const char* what() const throw() {
            return m_description.c_str();
        }

    private:
        std::string m_description;
};

class IOError : public PyMeshException {
    public:
        IOError(const std::string& description) :
            PyMeshException(description) {}
        virtual ~IOError() throw() {}
};

class RuntimeError : public PyMeshException {
    public:
        RuntimeError(const std::string& description) :
            PyMeshException(description) {}
        virtual ~RuntimeError() throw() {}
};

class NotImplementedError : public PyMeshException {
    public:
        NotImplementedError(const std::string& description) :
            PyMeshException(description) {}
        virtual ~NotImplementedError() throw() {}
};
}
