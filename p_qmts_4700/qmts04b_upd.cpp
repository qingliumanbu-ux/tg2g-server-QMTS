/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-11
Description: 转炉测厚标准信息修改
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 新增附件管理表
/// <para>
/// 获取输入参数：TQMTS04B(转炉测厚标准信息表) ；
/// </para>
/// <para>数据库表：TQMTS04B(转炉测厚标准信息表));
///
/// 前台TQMTS04B画面的F4(修改)调用    </para>
/// </summary>
/// <param name="TQMTS04B">转炉测厚标准信息表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts04b_upd)

int f_qmts04b_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";
	/* 实体类定义 */
	CModel tqmts04b("TQMTS04B");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	EIClass bcls_rec_snd;
	EIClass bcls_ret_snd;
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts04b.Reset();
			tqmts04b.MergeFrom(bcls_rec->Tables[0].Rows[i]);


			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT REC_CREATOR,REC_CREATE_TIME,ARCHIVE_FLAG "
					"   FROM tqmts04b "
					"  WHERE SEQ_NO = @SEQ_NO ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("SEQ_NO", tqmts04b["SEQ_NO"]);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts04b["REC_CREATOR"] = cmd_inq.GetString(1);
				tqmts04b["REC_CREATE_TIME"] = cmd_inq.GetString(2);
				tqmts04b["ARCHIVE_FLAG"] = cmd_inq.GetString(3);
			}
			cmd_inq.Close();
			/******************** 赋初值 *************************/
			tqmts04b["REC_REVISOR"] = s.userid;
			tqmts04b["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

			tqmts04b.Delete("SEQ_NO"); //条件字段项
			tqmts04b.TrimOrBlank();
			tqmts04b.Insert();

		}
		sprintf(s.msg, _RES("GCRSS0000018")/*新增信息失败。*/);

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
