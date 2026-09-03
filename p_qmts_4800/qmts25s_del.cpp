/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-13
Description: 实绩_工序成分后备删除
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///实绩_工序成分后备删除
/// <para>
/// 1.实绩_工序成分后备删除
/// 
/// </para>
/// <para>数据库表：TQMTS25(标准_工序成分)				</para>
/// <para>主调用函数：前台QMTS25S画面的F5(删除)调用		</para>
/// <para>需调用函数：									</para>
/// </summary>
/// <param name="HEAT_NO">熔炼号						</param>
/// <param name="ST_SAMPLE_NO">试样号					</param>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts25s_del)


int f_qmts25s_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	
	CDecimal v_count = 0;

	CString sqlstr = "";
	
	/* 实体类定义 */
	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");
	CModel tqmts23("TQMTS23");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{    
			/* 取得单行传入信息 */
			tqmts24.Reset();
			tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts25s_del IN:---heat_no[{0}]",(const char*)tqmts24["HEAT_NO"].ToString());
			Log::Trace("", "","qmts25s_del IN:---st_sample_no[{0}]",(const char*)tqmts24["ST_SAMPLE_NO"].ToString());

			//校验传入参数
			if(tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tqmts24["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0004260")/*请选择或输入试样号。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tqmts24["ST_SAMPLE_DIV"].ToString().Trim() == "1" && tqmts24["REP_ELM_SEL_FLAG"].ToString().Trim() == "1")
			{
				strcpy(s.msg, _RES("QM00S0006263")/*代表样不能删除。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts25["HEAT_NO"] = tqmts24["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			tqmts25.Delete("HEAT_NO,ST_SAMPLE_NO"); //条件字段项
			tqmts24.Delete("HEAT_NO,ST_SAMPLE_NO"); //条件字段项

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT ST_SAMPLE_DIV,REP_ELM_SEL_FLAG "
							 "  FROM TQMTS24 "
							 " WHERE HEAT_NO = @heat_no "
							 "   AND ST_SAMPLE_NO = @st_sample_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("st_sample_no", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts24["ST_SAMPLE_DIV"] = cmd_inq.GetString(1);
				tqmts24["REP_ELM_SEL_FLAG"] = cmd_inq.GetString(2);
			}
			cmd_inq.Close();
			Log::Trace("", "","st_sample_div[{0}]",(const char*)tqmts24["ST_SAMPLE_DIV"].ToString());
			Log::Trace("", "","rep_elm_sel_flag[{0}]",(const char*)tqmts24["REP_ELM_SEL_FLAG"].ToString());

			//修改炉次质量信息表的判定信息
			//if(tqmts24["ST_SAMPLE_DIV"].ToString().Trim() == "1" && tqmts24["REP_ELM_SEL_FLAG"].ToString().Trim() == "1")
			//{
			//	tqmts23["HEAT_NO"] = tqmts24["HEAT_NO"];
			//	tqmts23["JUDGE_CODE"] = " ";
			//	tqmts23["JUDGE_MAKER"] = " ";
			//	tqmts23["JUDGE_TIME"] = " ";
			//	tqmts23["JUDGE_REMARK"] = " ";
			//	tqmts23.Update("JUDGE_CODE,JUDGE_MAKER,JUDGE_TIME,JUDGE_REMARK",  //修改字段项
			//				"HEAT_NO"); //条件字段项
			//}
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


	//在函数退出前，统一Close()操作
	//cmd_inq1.Close();
	//cmd_del1.Close();
	//cmd_del2.Close();
	//cmd_inq2.Close();
	//cmd_del3.Close();
	//cmd_upd1.Close();

	return doFlag;
}
