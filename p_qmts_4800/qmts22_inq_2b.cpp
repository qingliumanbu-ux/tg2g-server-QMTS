/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      gx
Version:     1.0
Date:        2015-06-29
Description: 炉次质量信息查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



// service入口
BM2F_ENTERACE(qmts22_inq_2b)


int f_qmts22_inq_2b(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_slab_no = "";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts2b("TQMTS2B");
	
	CDbCommand cmd_inq(conn);

	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_CONTENT");
	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "ITEM_VALUES");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_JUDGE_FLAG");


	try
	{
		/*获得传入参数*/
		s_slab_no = bcls_rec->Tables[0].Rows[0]["slab_no"].ToString().Trim();

		Log::Trace("", "", "qmts22_inq_2b IN:---s_slab_no = [{0}]", s_slab_no);
		 
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS2B "
				"  WHERE SLAB_NO  = @slab_no "
			"  ORDER BY ST_SAMPLE_NO ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("slab_no", s_slab_no.Trim());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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

	return doFlag;
}

