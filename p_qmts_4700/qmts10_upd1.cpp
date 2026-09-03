/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-11
Description: 查询附件信息
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 修改附件管理表
/// <para>
/// 获取输入参数：TQMTS10(制造标准-附件管理表) ；
/// </para>
/// <para>数据库表：TQMTS10(制造标准-附件管理表));
///
/// 前台QMTS10画面的F4(修改)调用    </para>
/// </summary>
/// <param name="TQMTS10">附件管理表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts10_upd1)

int f_qmts10_upd1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CDecimal seq_no = 0;
	CString sqlstr = "";
	/* 实体类定义 */
	CModel tqmts10("TQMTS10");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	EIClass bcls_rec_snd;
	EIClass bcls_ret_snd;
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		
			tqmts10.Reset();
			tqmts10.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			seq_no = bcls_rec->Tables[0].Rows[0]["seq_no"].ToDecimal();
			tqmts10["ATTA_NAME"] = bcls_rec->Tables[1].Rows[0]["file_name"].ToString().Trim();
			Log::Trace("", "", "qmts10_upd1 IN:---ATTA_NAME = [{0}]", (const char*)tqmts10["ATTA_NAME"].ToString());
			Log::Trace("", "", "qmts10_upd1 IN:---SEQ_NO = [{0}]", seq_no);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT REC_CREATOR,REC_CREATE_TIME,ARCHIVE_FLAG "
					"   FROM tqmts10 "
					"  WHERE SEQ_NO = @seq_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("seq_no", seq_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts10["REC_CREATOR"] = cmd_inq.GetString(1);
				tqmts10["REC_CREATE_TIME"] = cmd_inq.GetString(2);
				tqmts10["ARCHIVE_FLAG"] = cmd_inq.GetString(3);
			}
			cmd_inq.Close();
			/******************** 赋初值 *************************/
			tqmts10["REC_REVISOR"] = s.userid;
			tqmts10["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

			//tqmts10.Delete("SEQ_NO"); //条件字段项
			//tqmts10.TrimOrBlank();
			//tqmts10.Insert();
			tqmts10.Update("ATTA_NAME","SEQ_NO");

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
