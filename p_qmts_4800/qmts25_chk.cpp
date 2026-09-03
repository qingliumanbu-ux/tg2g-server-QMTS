/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-13
Description: 试样成分实绩判定
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 试样成分实绩判定
/// <para>
/// 试样成分实绩判定。
/// 
/// </para>
/// <para>数据库表：TQMTS25(实绩_工序成分)			</para>
/// <para>主调用函数：前台QMTS25画面的F6(判定)调用  </para>
/// <para>需调用函数：								</para>
/// </summary>
/// <param name="PONO">制造命令号					</param>
/// <param name="ST_SAMPLE_NO">炼钢试样号			</param>
/// <param name="ST_NO">  出钢记号					</param>
/// <returns>  </returns>
===========================================================</remark>*/


/*  外部函数申明  */
int f_qmts_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//成分判定
//int f_qmts_yc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); 
// service入口
BM2F_ENTERACE(qmts25_chk)


int f_qmts25_chk(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString sqlstr = "";
	CString s_proc_no = "";

	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;

	
	EIClass inBlock1;
	EIClass outBlock1;

	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");
	

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/* 取得单行传入信息 */
			tqmts25.Reset();
			tqmts25.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "", "tqmts25_ins IN:---heat_no[{0}]", (const char*)tqmts25["HEAT_NO"].ToString());
			Log::Trace("", "", "tqmts25_ins IN:---st_sample_no[{0}]", (const char*)tqmts25["ST_SAMPLE_NO"].ToString());

			//校验传入参数
			if (tqmts25["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tqmts25["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0004260")/*请选择或输入试样号。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT ST_NO,PONO "
					"  FROM TQMTS24 "
					" WHERE HEAT_NO = @heat_no "
					"   AND ST_SAMPLE_NO = @st_sample_no";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts25["HEAT_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("st_sample_no", tqmts25["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts25["ST_NO"] = cmd_inq.GetString(1);
				tqmts25["PONO"] = cmd_inq.GetString(2);
			}
			else
			{
				sprintf(s.msg, "熔炼号[%s]试样号[%s]的成分不存在，请先新增！", (const char*)tqmts25["HEAT_NO"].ToString(), (const char*)tqmts25["ST_SAMPLE_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();
			Log::Trace("", "", "ST_NO[{0}]", (const char*)tqmts25["ST_NO"].ToString());

			//启动判定
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts25["HEAT_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts25["ST_SAMPLE_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts25["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts25["PONO"];
			doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f, conn);
			if (doFlag != 0)
			{
				Log::Trace("", "", "f_qmts_jud() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//---------------------------调用炉次等级计算，判断CC成分异常---------------------------
			//HYF 20130528 新增CC成分异常判定及炉次等级计算函数
			Log::Trace("", "", "成分异常判定");

			//查询炼钢计划号
			tqmts23["HEAT_NO"] = tqmts25["HEAT_NO"];
			tqmts23.Query("HEAT_NO");

			//查询CC的处理号
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT PROC_NO FROM "
					" (SELECT PROC_NO FROM TPSSM12 WHERE   SM_PLAN_NO = @sm_plan_no AND AREA_ID = '5' "/*AREA_ID:5-连铸 3-转炉 4-精炼*/
					" UNION "
					" SELECT PROC_NO FROM TPSSM42 WHERE  SM_PLAN_NO = @sm_plan_no AND AREA_ID = '5' ) "
					;
				Log::Trace("", "", "sqlstr[{0}]", sqlstr);
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("sm_plan_no", tqmts23["SM_PLAN_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				s_proc_no = cmd_inq.GetString(1);
				Log::Trace("", "", "s_proc_no[{0}]", s_proc_no);
			}
			cmd_inq.Close();

			inBlock1.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			inBlock1.Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
			inBlock1.Tables[0].Columns.Add(DT_STRING, "ST_NO");
			inBlock1.Tables[0].Columns.Add(DT_STRING, "PROC_NO");

			inBlock1.Tables[0].Rows.Add();
			inBlock1.Tables[0].Rows[0]["HEAT_NO"] = tqmts25["HEAT_NO"];
			inBlock1.Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = "C";
			inBlock1.Tables[0].Rows[0]["ST_NO"] = tqmts25["ST_NO"];
			inBlock1.Tables[0].Rows[0]["PROC_NO"] = s_proc_no;

			//doFlag = f_qmts_yc(&inBlock1, &outBlock1, conn);

			Log::Trace("", "", "ST_NO[{0}]doFlag[{1}]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", (const char*)tqmts25["ST_NO"].ToString(), doFlag);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
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
