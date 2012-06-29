/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#include <boost/format.hpp>
#include "DBGWCommon.h"
#include "DBGWError.h"
#include "DBGWValue.h"

namespace dbgw
{

  static const int MAX_ERROR_MESSAGE_SIZE = 2048;

  __thread int g_nErrorCode;
  __thread int g_nInterfaceErrorCode;
  __thread char g_szErrorMessage[MAX_ERROR_MESSAGE_SIZE];
  __thread char g_szFormattedErrorMessage[MAX_ERROR_MESSAGE_SIZE];

  static void setErrorMessage(char *szTarget, const char *szErrorMessage)
  {
    if (szErrorMessage == NULL)
      {
        strcpy(szTarget, "");
      }
    else
      {
        int nErrorMessageSize = strlen(szErrorMessage);
        if (nErrorMessageSize > MAX_ERROR_MESSAGE_SIZE - 1)
          {
            nErrorMessageSize = MAX_ERROR_MESSAGE_SIZE - 1;
          }
        memcpy(szTarget, szErrorMessage, nErrorMessageSize);
        szTarget[nErrorMessageSize] = '\0';
      }
  }

  void setLastException(const DBGWInterfaceException &exception)
  {
    g_nErrorCode = exception.getErrorCode();
    g_nInterfaceErrorCode = exception.getInterfaceErrorCode();
    setErrorMessage(g_szErrorMessage, exception.getErrorMessage());
    setErrorMessage(g_szFormattedErrorMessage, exception.what());
  }

  void clearException()
  {
    g_nErrorCode = DBGWErrorCode::NO_ERROR;
    g_nInterfaceErrorCode = DBGWErrorCode::NO_ERROR;
    strcpy(g_szErrorMessage, "");
    strcpy(g_szFormattedErrorMessage, "");
  }

  DBGWInterfaceException getLastException()
  {
    if (g_nInterfaceErrorCode != DBGWErrorCode::NO_ERROR)
      {
        return DBGWInterfaceException(g_nInterfaceErrorCode, g_szErrorMessage);
      }
    else
      {
        return DBGWException(g_nErrorCode, g_szErrorMessage);
      }
  }

  int getLastErrorCode()
  {
    return g_nErrorCode;
  }

  int getLastInterfaceErrorCode()
  {
    return g_nInterfaceErrorCode;
  }

  const char *getLastErrorMessage()
  {
    return g_szErrorMessage;
  }

  const char *getFormattedErrorMessage()
  {
    return g_szFormattedErrorMessage;
  }

  DBGWException::DBGWException() throw() :
    m_nErrorCode(DBGWErrorCode::NO_ERROR)
  {
  }

  DBGWException::DBGWException(int nErrorCode) throw() :
    m_nErrorCode(nErrorCode)
  {
    createErrorMessage();
  }

  DBGWException::DBGWException(int nErrorCode, const string &errorMessage) throw() :
    m_nErrorCode(nErrorCode), m_errorMessage(errorMessage)
  {
    createErrorMessage();
  }

  DBGWException::DBGWException(const DBGWException &exception) throw() :
    std::exception(exception), m_nErrorCode(exception.m_nErrorCode),
    m_errorMessage(exception.m_errorMessage), m_what(exception.m_what)
  {
  }

  DBGWException::DBGWException(const std::exception &exception) throw() :
    std::exception(exception),
    m_nErrorCode(DBGWErrorCode::EXTERNAL_STANDARD_ERROR),
    m_errorMessage(exception.what()), m_what(exception.what())
  {
  }

  DBGWException::~DBGWException() throw()
  {
  }

  int DBGWException::getErrorCode() const
  {
    return m_nErrorCode;
  }

  const char *DBGWException::getErrorMessage() const
  {
    return m_errorMessage.c_str();
  }

  const char *DBGWException::what() const throw()
  {
    return m_what.c_str();
  }

  void DBGWException::createErrorMessage()
  {
    doCreateErrorMessage();
  }

  void DBGWException::doCreateErrorMessage()
  {
    stringstream buffer;
    buffer << "[" << m_nErrorCode << "] " << m_errorMessage;
    m_what = buffer.str();
  }

  DBGWInterfaceException::DBGWInterfaceException() throw() :
    DBGWException(), m_nInterfaceErrorCode(DBGWErrorCode::NO_ERROR)
  {
  }

  DBGWInterfaceException::DBGWInterfaceException(const string &errorMessage) throw() :
    DBGWException(DBGWErrorCode::INTERFACE_ERROR, errorMessage),
    m_nInterfaceErrorCode(DBGWErrorCode::NO_ERROR)
  {
  }

  DBGWInterfaceException::DBGWInterfaceException(int nInterfaceErrorCode) throw() :
    DBGWException(DBGWErrorCode::INTERFACE_ERROR),
    m_nInterfaceErrorCode(nInterfaceErrorCode)
  {
  }

  DBGWInterfaceException::DBGWInterfaceException(int nInterfaceErrorCode,
      const string &errorMessage) throw() :
    DBGWException(DBGWErrorCode::INTERFACE_ERROR, errorMessage),
    m_nInterfaceErrorCode(nInterfaceErrorCode)
  {
  }

  DBGWInterfaceException::DBGWInterfaceException(const DBGWException &exception) throw() :
    DBGWException(exception), m_nInterfaceErrorCode(DBGWErrorCode::NO_ERROR)
  {
  }

  DBGWInterfaceException::DBGWInterfaceException(
      const DBGWInterfaceException &exception) throw() :
    DBGWException(exception),
    m_nInterfaceErrorCode(exception.m_nInterfaceErrorCode)
  {
  }

  DBGWInterfaceException::DBGWInterfaceException(
      const std::exception &exception) throw() :
    DBGWException(exception), m_nInterfaceErrorCode(DBGWErrorCode::NO_ERROR)
  {
  }

  int DBGWInterfaceException::getInterfaceErrorCode() const
  {
    return m_nInterfaceErrorCode;
  }

  bool DBGWInterfaceException::isConnectionError() const
  {
    return true;
  }

  NotExistNamespaceException::NotExistNamespaceException(const char *szNamespace) throw() :
    DBGWException(
        DBGWErrorCode::CONF_NOT_EXIST_NAMESPACE,
        (boost::format("The %s namespace is not exist.")
            % szNamespace).str())
  {
  }

  NotExistQueryInXmlException::NotExistQueryInXmlException(const char *szSqlName) throw() :
    DBGWException(
        DBGWErrorCode::CONF_NOT_EXIST_QUERY_IN_XML,
        (boost::format("The %s query is not exist in xml.")
            % szSqlName).str())
  {
  }

  NotExistAddedHostException::NotExistAddedHostException() throw() :
    DBGWException(DBGWErrorCode::CONF_NOT_EXIST_ADDED_HOST,
        "There is no added host.")
  {
  }

  FetchHostFailException::FetchHostFailException() throw() :
    DBGWException(DBGWErrorCode::CONF_FETCH_HOST_FAIL, "Fetch host fail.")
  {
  }

  NotYetLoadedException::NotYetLoadedException() throw() :
    DBGWException(DBGWErrorCode::CONF_NOT_YET_LOADED,
        "Configuration is not yet loaded.")
  {
  }

  NotExistVersionException::NotExistVersionException(int nVersion) throw() :
    DBGWException(
        DBGWErrorCode::CONF_NOT_EXIST_VERSION,
        (boost::format(
            "The configuration of version %d is not exist.")
            % nVersion).str())
  {

  }

  NotExistConnException::NotExistConnException(const char *szGroupName) throw() :
    DBGWException(
        DBGWErrorCode::SQL_NOT_EXIST_CONN,
        (boost::format("The %s connection group is not exist.")
            % szGroupName).str())
  {
  }

  InvalidSqlException::InvalidSqlException(const char *szFileName,
      const char *szSqlName) throw() :
    DBGWException(
        DBGWErrorCode::SQL_INVALID_SQL,
        (boost::format("Cannot parse sql %s in %s.") % szSqlName
            % szFileName).str())
  {
  }

  NotExistParamException::NotExistParamException(int nIndex) throw() :
    DBGWException(
        DBGWErrorCode::SQL_NOT_EXIST_PARAM,
        (boost::format(
            "The bind parameter (index : %d) is not exist.")
            % nIndex).str())
  {
  }

  NotExistParamException::NotExistParamException(string name) throw() :
    DBGWException(
        DBGWErrorCode::SQL_NOT_EXIST_PARAM,
        (boost::format("The bind parameter (key : %s) is not exist.")
            % name).str())
  {
  }

  ExecuteBeforePrepareException::ExecuteBeforePrepareException() throw() :
    DBGWException(DBGWErrorCode::SQL_EXECUTE_BEFORE_PREPARE,
        "The query is executed before prepare.")
  {
  }

  NotExistSetException::NotExistSetException(const char *szKey) throw() :
    DBGWException(DBGWErrorCode::VALUE_NOT_EXIST_SET,
        (boost::format("The %s key is not exist in set.") % szKey).str())
  {
  }

  NotExistSetException::NotExistSetException(size_t nIndex) throw() :
    DBGWException(
        DBGWErrorCode::VALUE_NOT_EXIST_SET,
        (boost::format(
            "The value of position %d is not exist in set.")
            % nIndex).str())
  {
  }

  MismatchValueTypeException::MismatchValueTypeException(int orgType,
      int convType) throw() :
    DBGWException(
        DBGWErrorCode::VALUE_MISMATCH_VALUE_TYPE,
        (boost::format("Cannot cast %s to %s.")
            % getDBGWValueTypeString(orgType)
            % getDBGWValueTypeString(convType)).str())
  {
  }

  InvalidValueTypeException::InvalidValueTypeException(int type) throw() :
    DBGWException(DBGWErrorCode::VALUE_INVALID_VALUE_TYPE,
        (boost::format("The value type %d is invalid.") % type).str())
  {
  }

  InvalidValueTypeException::InvalidValueTypeException(const char *szType) throw() :
    DBGWException(DBGWErrorCode::VALUE_INVALID_VALUE_TYPE,
        (boost::format("The value type %s is invalid.") % szType).str())
  {
  }

  InvalidValueFormatException::InvalidValueFormatException(const char *szType,
      const char *szFormat) throw() :
    DBGWException(DBGWErrorCode::VALUE_INVALID_VALUE_TYPE,
        (boost::format("The %s is not valid %s type.") % szFormat % szType).str())
  {
  }

  MultisetIgnoreResultFlagFalseException::MultisetIgnoreResultFlagFalseException(
      const char *szSqlName) throw() :
    DBGWException(
        DBGWErrorCode::CLIENT_MULTISET_IGNORE_FLAG_FALSE,
        (boost::format(
            "The 'ignore_result' flag should be set false only once in %s.")
            % szSqlName).str())
  {
  }

  InvalidClientException::InvalidClientException() throw() :
    DBGWException(DBGWErrorCode::CLIENT_INVALID_CLIENT,
        "The client is invalid.")
  {
  }

  NotAllowedNextException::NotAllowedNextException() throw() :
    DBGWException(DBGWErrorCode::RESULT_NOT_ALLOWED_NEXT,
        "The next() operation is allowed only select query.")
  {
  }

  NotAllowedGetMetadataException::NotAllowedGetMetadataException() throw() :
    DBGWException(DBGWErrorCode::RESULT_NOT_ALLOWED_GET_METADATA,
        "Only Select query is able to make metadata list.")
  {
  }

  NotAllowedOperationException::NotAllowedOperationException(
      const char *szOperation, const char *szQueryType) throw() :
    DBGWException(
        DBGWErrorCode::RESULT_NOT_ALLOWED_OPERATION,
        (boost::format(
            "The %s operation is only allowed for query type %s.")
            % szOperation % szQueryType).str())
  {
  }

  ValidateFailException::ValidateFailException() throw() :
    DBGWException(DBGWErrorCode::RESULT_VALIDATE_FAIL,
        "The result type is different.")
  {
  }

  ValidateFailException::ValidateFailException(const DBGWException &exception) throw() :
    DBGWException(
        DBGWErrorCode::RESULT_VALIDATE_FAIL,
        (boost::format(
            "Some of group is failed to execute query. %s")
            % exception.what()).str())
  {
  }

  ValidateFailException::ValidateFailException(int lhs, int rhs) throw() :
    DBGWException(
        DBGWErrorCode::RESULT_VALIDATE_FAIL,
        (boost::format(
            "The affected row count is different each other. %d != %d")
            % lhs % rhs).str())
  {
  }

  ValidateTypeFailException::ValidateTypeFailException(const char *szName,
      const string &lhs, const char *szLhsType, const string &rhs,
      const char *szRhsType) throw() :
    DBGWException(
        DBGWErrorCode::RESULT_VALIDATE_TYPE_FAIL,
        (boost::format(
            "The %s's type is different each other. %s (%s) != %s (%s)")
            % szName % lhs % szLhsType % rhs % szRhsType).str())
  {
  }

  ValidateValueFailException::ValidateValueFailException(const char *szName,
      const string &lhs) throw() :
    DBGWException(
        DBGWErrorCode::RESULT_VALIDATE_FAIL,
        (boost::format(
            "The %s's value is different each other. %s != NULL")
            % szName % lhs).str())
  {
  }

  ValidateValueFailException::ValidateValueFailException(const char *szName,
      const string &lhs, const string &rhs) throw() :
    DBGWException(
        DBGWErrorCode::RESULT_VALIDATE_VALUE_FAIL,
        (boost::format("The %s's value is different each other. %s != %s")
            % szName % lhs % rhs).str())
  {
  }

  CreateFailParserExeception::CreateFailParserExeception(const char *szFileName) throw() :
    DBGWException(
        DBGWErrorCode::XML_FAIL_CREATE_PARSER,
        (boost::format("Cannot create xml parser from %s.")
            % szFileName).str())
  {
  }

  DuplicateNamespaceExeception::DuplicateNamespaceExeception(
      const string &nameSpace, const string &fileNameNew,
      const string &fileNameOld) throw() :
    DBGWException(
        DBGWErrorCode::XML_DUPLICATE_NAMESPACE,
        (boost::format(
            "The namspace %s in %s is already exist in %s.")
            % nameSpace % fileNameNew % fileNameOld).str())
  {
  }

  DuplicateSqlNameException::DuplicateSqlNameException(const char *szSqlName,
      const char *szFileNameNew, const char *szFileNameOld) throw() :
    DBGWException(
        DBGWErrorCode::XML_DUPLICATE_SQLNAME,
        (boost::format("The %s in %s is already exist in %s.")
            % szSqlName % szFileNameNew % szFileNameOld).str())
  {
  }

  DuplicateGroupNameException::DuplicateGroupNameException(
      const string &groupName, const string &fileNameNew,
      const string &fileNameOld) throw() :
    DBGWException(
        DBGWErrorCode::XML_DUPLICATE_GROUPNAME,
        (boost::format("The %s in %s is already exist in %s.")
            % groupName % fileNameNew % fileNameOld).str())
  {
  }

  NotExistNodeInXmlException::NotExistNodeInXmlException(const char *szNodeName,
      const char *szXmlFile) throw() :
    DBGWException(
        DBGWErrorCode::XML_NOT_EXIST_NODE,
        (boost::format("The %s node is not exist in %s.")
            % szNodeName % szXmlFile).str())
  {
  }

  NotExistPropertyException::NotExistPropertyException(const char *szNodeName,
      const char *szPropName) throw() :
    DBGWException(
        DBGWErrorCode::XML_NOT_EXIST_PROPERTY,
        (boost::format("Cannot find %s property of %s node.")
            % szPropName % szNodeName).str())
  {
  }

  InvalidPropertyValueException::InvalidPropertyValueException(
      const char *szValue, const char *szCorrectValueSet) throw() :
    DBGWException(
        DBGWErrorCode::XML_INVALID_PROPERTY_VALUE,
        (boost::format("The value of property %s have to be [%s].")
            % szValue % szCorrectValueSet).str())
  {
  }

  InvalidXMLSyntaxException::InvalidXMLSyntaxException(
      const char *szXmlErrorMessage, const char *szFileName, int nLine, int nCol) throw() :
    DBGWException(DBGWErrorCode::XML_INVALID_SYNTAX,
        (boost::format("%s in %s, line %d, column %d") % szXmlErrorMessage
            % szFileName % nLine % nCol).str())
  {
  }

  MutexInitFailException::MutexInitFailException() throw() :
    DBGWException(DBGWErrorCode::EXTERNAL_MUTEX_INIT_FAIL,
        "Failed to init mutex object.")
  {
  }

  MemoryAllocationFail::MemoryAllocationFail(int nSize) throw() :
    DBGWException(DBGWErrorCode::EXTERNAL_MEMORY_ALLOC_FAIL,
        (boost::format("Failed to allocate memory size (%d).") % nSize).str())
  {
  }

}
