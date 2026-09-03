/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   刘家岩
Version:    1.0
Date:     2012-02-16
Description: 工序成分标准表中出钢记号查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 工序成分标准表中出钢记号查询
/// <para>
/// 获取输入参数：TQMTS02(成分标准) ；
/// </para>
/// <para>数据库表：TQMTS02(成分标准)); 
///  前台画面QMTS02中出钢记号下拉框调用  </para>
/// </summary>
/// <param name="TQMTS02">成分标准    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE(qmts02_inq_st)

int f_qmts02_inq_st(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;

	CString i_st_no = " ";
	CDecimal  v_count;
	CString v_factory_div = " ";
	
	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获得输入参数
		v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();

		//输入参数校验
		if (v_factory_div.Trim() == "")
		{
			sprintf(s.msg,_RES("QM00S0004175")/*传入厂别区分不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//定义数据库操作命令对象comm_inq执行sql语句，sql字符串用""包括，可以分行，但每行前后务必留出一个空格。
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = " SELECT DISTINCT st_no "
						 " FROM TQMTS02 "						   
						 " WHERE factory_div = @factory_div "
						 " ORDER BY st_no ASC ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
		cmd_inq.Parameters.Set("factory_div", v_factory_div.Trim());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	//在函数退出前，统一Close()操作

	return doFlag;
}
