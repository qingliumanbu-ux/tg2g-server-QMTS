/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      wsl
Version:     1.0
Date:        2023-12-13
Description: 试样成分实绩后备-炉次查询 翻新ag
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
///试样成分实绩后备-炉次查询
/// <para>
/// 1.试样成分实绩后备-炉次查询
/// </para>
/// <para>数据库表：tpssm11(炼钢作业计划炉次钢种管理表)		</para>
/// <para>主调用函数：前台QMBM25S的F2查询				    </para>
/// <para>需调用函数：										</para>
/// </summary>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmbs25s_h5_inq)


int f_qmbs25s_h5_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	/* 实体类定义 */
	CModel tpssm11("TPSSM11");
	CString sqlstr = "";
	CString sql = "";
	CString sql_where = "";
	CString sql_order_by = "";
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);


	try
	{
		/* 获得输入参数 */
		tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tpssm11["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();

		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}

		Log::Trace("", "", "qmbs25s_inq IN:---HEAT_NO = [{0}]", tpssm11["HEAT_NO"].ToString());
		Log::Trace("", "", "qmbs25s_inq IN:---PONO = [{0}]", tpssm11["PONO"].ToString());
		Log::Trace("", "", "qmbs25s_inq IN:record_count_per_page--- = [{0}]", record_count_per_page);
		Log::Trace("", "", "qmbs25s_inq IN:current_page_no--- = [{0}]", current_page_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sql = " SELECT * FROM ( "
				"SELECT A.HEAT_NO, A.PONO, A.SM_PLAN_NO, B.JUDGE_ST_NO ST_NO, A.REFINE_ROUTE_CODE, A.FACTORY_DIV,A. CAST_NO || '-' || A.CAST_DIV_NO CAST_NO_SHOW, "
				"A.PONO_STATUS, A.CAST_NO, A.CAST_DIV_NO, B.REP_ELM_SEL_FLAG FROM (SELECT * FROM TPSSM11 UNION SELECT * FROM TPSSM41) A  "
				"LEFT JOIN TQMTS23 B ON A.HEAT_NO = B.HEAT_NO AND A.PONO = B.PONO) ";

			 sql_where = "WHERE 1 = 1 ";
			
			if (tpssm11["HEAT_NO"].ToString().Trim() != "")	 sql_where += " AND HEAT_NO LIKE '%" + tpssm11["HEAT_NO"].ToString().Trim() + "%' ";
			if (tpssm11["PONO"].ToString().Trim() != "")	sql_where += " AND  PONO LIKE '%" + tpssm11["PONO"].ToString().Trim() + "%'  ";
			sql_where += " AND HEAT_NO <> ' ' ";
		    sql_order_by ="ORDER BY CAST_NO  ASC, CAST_DIV_NO ASC ";
			break;
		}

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";

		Log::Trace("", "", "sqlstr=[{0}]", sqlstr);
		/*连接sql语句*/
		sqlstr = sql_count + sql + sql_where + ")";
		cmd_inq.SetCommandText(sqlstr);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		/*完成拼接查询sql*/
		sqlstr = sql + sql_where + sql_order_by;
		Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();


		//返回分页信息
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
