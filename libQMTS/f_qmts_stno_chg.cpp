/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-19
Description: 钢种变更的成分实绩修改，重新判定该工序的化学成分
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///钢种变更的成分实绩修改，重新判定该工序的化学成分
/// <para>
/// 1.钢种变更的成分实绩修改，重新判定该工序的化学成分。
/// 
/// </para>
/// <para>数据库表：TQMTS25(成分实绩表)				</para>
/// <para>主调用函数：								</para>
/// <para>需调用函数： f_qmts_jud					</para>
/// </summary>
/// <param name="PONO_IN">新制造命令号				</param>
/// <param name="PONO_OUT">原制造命令号				</param>
/// <param name="ST_NO">出钢记号					</param>
/// <param name="WHOLE_BACKLOG_CODE">变更工序       </param>
/// <returns>  </returns>
===========================================================</remark>*/


/*  外部函数申明  */

BM2_FUNCTION_IMPORT
 int f_qmts_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//成分判定


BM2_FUNCTION_EXPORT
 int f_qmts_stno_chg(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts24("TQMTS24");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		//获得输入参数
		CString v_pono_old = bcls_rec->Tables[0].Rows[0]["PONO_OUT"].ToString().TrimOrBlank();
		CString v_pono_new = bcls_rec->Tables[0].Rows[0]["PONO_IN"].ToString().TrimOrBlank();
		CString v_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank();
		CString v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO_IN"].ToString().TrimOrBlank();
		CString v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();

		Log::Trace("", __FUNCTION__, "f_qmts_stno_chg IN:---v_pono_old[{0}]",(const char*)v_pono_old);
		Log::Trace("", __FUNCTION__, "f_qmts_stno_chg IN:---v_pono_new[{0}]",(const char*)v_pono_new);
		Log::Trace("", __FUNCTION__, "f_qmts_stno_chg IN:---v_whole_backlog_code[{0}]",(const char*)v_whole_backlog_code);
		Log::Trace("", __FUNCTION__, "f_qmts_stno_chg IN:---v_st_no[{0}]",(const char*)v_st_no);
		Log::Trace("", __FUNCTION__, "f_qmts_stno_chg IN:---v_factory_div[{0}]", (const char*)v_factory_div);

		//*********************************   1. 获取/检查输入参数   *********************************//
		if(v_pono_old.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[原PONO号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(v_pono_new.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[新PONO号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(v_whole_backlog_code.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[工位代码不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(v_st_no.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[出钢记号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (v_factory_div.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[主工序代码不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//--------------------------------------------
		//钢种变更时，所有试样的PONO都更新
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "UPDATE TQMTS25 "
				"   SET PONO = @pono_new "
				" WHERE PONO = @pono_old ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);		
		cmd_upd.Parameters.Set("pono_new", v_pono_new);
		cmd_upd.Parameters.Set("pono_old", v_pono_old);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		//当前工序的钢种及判定结果更新
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "UPDATE TQMTS25 "
				"   SET ST_NO  = @st_no, "
				"       ELM_OK = 0 "
				" WHERE PONO = @pono_new "
				"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);		
		cmd_upd.Parameters.Set("st_no", v_st_no);
		cmd_upd.Parameters.Set("pono_new", v_pono_new);
		cmd_upd.Parameters.Set("whole_backlog_code", v_whole_backlog_code);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		//试样主表更新
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "UPDATE TQMTS24 "
				"   SET pono = @pono_new "
				" WHERE pono = @pono_old ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);		
		cmd_upd.Parameters.Set("pono_new", v_pono_new);
		cmd_upd.Parameters.Set("pono_old", v_pono_old);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "UPDATE TQMTS24 "
				"   SET ST_NO  = @st_no, "
				"       JUDGE_CODE = ' ' "
				" WHERE PONO = @pono_new "
				"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);		
		cmd_upd.Parameters.Set("st_no", v_st_no);
		cmd_upd.Parameters.Set("pono_new", v_pono_new);
		cmd_upd.Parameters.Set("whole_backlog_code", v_whole_backlog_code);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS24 "
				" WHERE PONO = @pono_new "
				"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
				" ORDER BY ST_SAMPLE_NO ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("pono_new", v_pono_new);
		cmd_inq.Parameters.Set("whole_backlog_code", v_whole_backlog_code);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts24);

			//==================================================================================================
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts24["HEAT_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts24["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts24["PONO"];

			//试样良的话参与成分判定
			if(tqmts24["SAMPLE_IFGOOD"].ToString() =="1" && (tqmts24["ST_SAMPLE_DIV"].ToString() =="1" || tqmts24["ST_SAMPLE_DIV"].ToString() =="4"))
			{
				doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f,conn);	  
				if(doFlag != 0)
				{
					Log::Trace("", __FUNCTION__, "f_qmts_jud() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		cmd_inq.Close();

		bcls_ret->Tables[0].Columns.Clear();
		bcls_ret->Tables[0].Columns.Add(DT_INT16,"CHK_FLAG");
		CDataRow& row_ret = bcls_ret->Tables[0].Rows.Add();
		row_ret["CHK_FLAG"] = 0;
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

