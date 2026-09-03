/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-28
Description: 查询组合元素公式代码
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 查询组合元素公式代码
/// <para>
/// 1.查询组合元素公式代码
/// </para>
/// <para>数据库表：tep0002(代码值集信息表)                </para>
/// <para>主调用函数：前台QMTSZZ画面的F2(查询)调用。       </para>
/// </summary>
/// <param name="CODE_CLASS"> 代码编号                     </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */

// service入口
BM2F_ENTERACE(qmtszz_inq)

int f_qmtszz_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_tep0002(conn);

	bcls_ret->Tables[0].Columns.Add(tep0002);

	try
	{
		/*获得传入参数*/
		tep0002.Reset();
		tep0002.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Trace("", "","qmtszz_inq IN:---CODE_CLASS = [{0}]",(const char*)tep0002["CODE_CLASS"].ToString());

		//查询工艺卡表
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TEP0002 "
				"  WHERE CODE_CLASS = @tep0002.CODE_CLASS ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tep0002.CODE_CLASS", tep0002["CODE_CLASS"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tep0002);

			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT CODE_DESC_1_CONTENT "
					"   FROM TEP0002_RES "
					"  WHERE CULTURE = 'zh_Hans' "
					"    AND CODE_CLASS = @tep0002.CODE_CLASS "
					"    AND CODE = @tep0002.CODE ";
				break;
			}
			cmd_inq_tep0002.SetCommandText(sqlstr);
			cmd_inq_tep0002.Parameters.Set("tep0002.CODE_CLASS", tep0002["CODE_CLASS"].ToString());
			cmd_inq_tep0002.Parameters.Set("tep0002.CODE", tep0002["CODE"].ToString());
			cmd_inq_tep0002.ExecuteReader();
			if(cmd_inq_tep0002.Read())
			{
				tep0002["CODE_DESC_1_CONTENT"] = cmd_inq_tep0002.GetString(1);
			}
			cmd_inq_tep0002.Close();

			CDataRow& row = bcls_ret->Tables[0].Rows.Add();
			row.Merge(tep0002);
		}
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
