/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      易玲
Version:     1.0
Date:        2016-04-07
Description: 查询工艺卡制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 查询工艺卡
/// <para>
/// 1.查询工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0X画面的F2(查询)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts0x_inq_ts)

int f_qmts0x_inq_ts(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString q_table_idx(" ");
	CString q_factory_div(" ");
	CString q_st_no("");
	CString s_table_name(" ");

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_tep0002(conn);

	try
	{
		/*获得传入参数*/
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_table_idx = bcls_rec->Tables[0].Rows[0]["table_idx"].ToString().TrimOrBlank();
		/////字符型DB2数据库是个null, oracle是个空格，update by yiling 20160620,主键不好改成动态SQL查询
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容	
			q_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			q_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
			break;
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			q_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();
			break;
		}
		Log::Trace("", "", "qmts0x_inq_ts IN:---q_table_idx = [{0}]", q_table_idx);
		Log::Trace("", "", "qmts0x_inq_ts IN:---ST_NO = [{0}]", (const char*)q_st_no);
		Log::Trace("", "", "qmts0x_inq_ts IN:---q_factory_div = [{0}]", (const char*)q_factory_div);


		tep0002["CODE_CLASS"] = "QMZ7";
		tep0002["CODE_DESC_2_CONTENT"] = q_table_idx;
		tep0002.Query("CODE_CLASS,CODE_DESC_2_CONTENT");
		s_table_name = tep0002["CODE_DESC_4_CONTENT"];
		Log::Trace("", "", "s_table_name[{0}]", (const char*)s_table_name);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM  "
				+ s_table_name +
				"  WHERE FACTORY_DIV = @q_factory_div "
				"    AND ST_NO LIKE @q_st_no||'%' "
		       ;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("q_factory_div", q_factory_div);
		cmd_inq.Parameters.Set("q_st_no", q_st_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
