/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-25
Description: 炉次确定代表成分
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
#include "tqmts24.h"
#include "tqmts25.h"
#include "tqmts29.h"
#include "tqmtqq0.h"

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次确定代表成分
/// <para>
/// 1.炉次确定代表成分。
/// 
/// </para>
/// <para>数据库表：TQMTS25(成分实绩表)                    </para>
/// <para>          TQMTS29(代表成分实绩表)			       </para>
/// <para>主调用函数： f_qmts_jud_01(推算板坯最终出钢记号) </para>
/// <para>需调用函数：
/// </summary>
/// <returns>  </returns>
===========================================================</remark>*/


BM2_FUNCTION_EXPORT
 int f_qmts_rep_elm_confm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CTQMTS24 tqmts24(conn);
	CTQMTS25 tqmts25(conn);
	CTQMTS29 tqmts29(conn);
	CTQMTQQ0 tqmtqq0(conn);

	CString sqlstr("");

	//CDbCommand对象的定义，统一放在Service或函数前段
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_tqmts25(conn);
	CDbCommand cmd_tep0002(conn);

	try
	{
		/*获得传入参数*/
		tqmts24.PONO = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		tqmts24.FACTORY_DIV = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "f_qmts_rep_elm_confm IN:---PONO = [{0}]",(const char*)tqmts24.PONO);
		Log::Trace("", __FUNCTION__, "f_qmts_rep_elm_confm IN:---FACTORY_DIV = [{0}]",(const char*)tqmts24.FACTORY_DIV);

		if(tqmts24.PONO.Trim() == "")
		{
			sprintf(s.sysmsg,_RES("GCRSS0000028")/*PONO号不能为空。*/);
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts24.FACTORY_DIV.Trim() == "")
		{
			sprintf(s.sysmsg,_RES("QM00S0004396")/*主工序代码不可为空。*/);
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS24 "
				"  WHERE  "
				"      PONO = @tqmts24.PONO "
				"    AND REP_ELM_SEL_FLAG = '1' ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("tqmts24.FACTORY_DIV", tqmts24.FACTORY_DIV);
		cmd_inq.Parameters.Set("tqmts24.PONO", tqmts24.PONO);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts24);

			Log::Trace("", __FUNCTION__, "st_sample_no = [{0}]",(const char*)tqmts24.ST_SAMPLE_NO);

			if(tqmts24.ST_SAMPLE_DIV=="1")//钢样
			{
				fetchRowCount = 0;
				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TQMTS25 "
						"  WHERE PONO = @tqmts24.PONO "
						"    AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO "
						"    AND ELM_CODE NOT IN ('001','014','016') "
						"  ORDER BY ELM_CODE ASC ";
					break;
				}
				cmd_tqmts25.SetCommandText(sqlstr);				
				cmd_tqmts25.Parameters.Set("tqmts24.PONO", tqmts24.PONO);
				cmd_tqmts25.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24.ST_SAMPLE_NO);
				cmd_tqmts25.ExecuteReader();
				while (cmd_tqmts25.Read())
				{
					cmd_tqmts25.Fetch(tqmts25);

					fetchRowCount++;

					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT CODE_DESC_2_CONTENT "
							"   FROM TEP0002 "
							"  WHERE CODE_CLASS = 'QMYS' "
							"    AND CODE = @tqmts29.ELM_CODE ";
						break;
					}
					cmd_tep0002.SetCommandText(sqlstr);
					cmd_tep0002.Parameters.Set("tqmts29.ELM_CODE", tqmts29.ELM_CODE);
					cmd_tep0002.ExecuteReader();
					if (cmd_tep0002.Read())
					{
						tqmts29.ELM_POS = CDecimal::Parse(cmd_tep0002.GetString(1));
						tqmtqq0.ELM_POS = CDecimal::Parse(cmd_tep0002.GetString(1));
					}
					cmd_tep0002.Close();

					/*新增TQMTS29*/
					tqmts29.REC_CREATOR = s.userid;
					tqmts29.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts29.REC_REVISOR = " ";
					tqmts29.REC_REVISE_TIME = " ";
					tqmts29.ARCHIVE_FLAG = " ";
					tqmts29.PONO = tqmts25.PONO;
					tqmts29.HEAT_NO = tqmts25.HEAT_NO;
					tqmts29.ST_NO = tqmts25.ST_NO;
					tqmts29.ELM_CODE = tqmts25.ELM_CODE;
					tqmts29.ELM_NAME = tqmts25.ELM_NAME;
					tqmts29.ELM_VALUE = tqmts25.ELM_ACT;
					tqmts29.ELM_OK = tqmts25.ELM_OK;
					tqmts29.ST_SAMPLE_TYPE = tqmts25.ST_SAMPLE_TYPE;
					tqmts29.ELM_BACKUP = tqmts25.ELM_BACKUP;
					tqmts29.FACTORY_DIV = tqmts24.FACTORY_DIV;

					tqmts29.ELM_UNIT = "%";

					tqmts29.Delete("HEAT_NO, ELM_CODE"); //条件字段项
					tqmts29.Insert();

					/*新增TQMTQQ0*/
					tqmtqq0.REC_CREATOR = s.userid;
					tqmtqq0.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmtqq0.REC_REVISOR = " ";
					tqmtqq0.REC_REVISE_TIME = " ";
					tqmtqq0.ARCHIVE_FLAG = " ";
					tqmtqq0.PONO = tqmts25.PONO;
					tqmtqq0.HEAT_NO = tqmts25.HEAT_NO;
					tqmtqq0.VENDOR_CODE = " ";
					tqmtqq0.VENDOR_NAME = " ";
					tqmtqq0.ST_NO = tqmts25.ST_NO;
					tqmtqq0.SAMPLE_LOT_NO = " ";
					tqmtqq0.ELM_CODE = tqmts25.ELM_CODE;
					tqmtqq0.ELM_NAME = tqmts25.ELM_NAME;
					tqmtqq0.ELM_ACT = tqmts25.ELM_ACT;
					tqmtqq0.ELM_UNIT = tqmts29.ELM_UNIT;
					tqmtqq0.PCH_JUDGE_CODE = " ";

					tqmtqq0.Delete("PONO, ELM_CODE"); //条件字段项
					Log::Trace("", __FUNCTION__, "tqmtqq0.PONO = [{0}]tqmtqq0.ELM_CODE[{1}]", (const char*)tqmtqq0.PONO, tqmtqq0.ELM_CODE);
					tqmtqq0.Insert();
				}
				cmd_tqmts25.Close();
			}
			if(fetchRowCount == 0)
			{
				CFormattable arguments[] = {(const char*)tqmts24.PONO,(const char*)tqmts24.ST_SAMPLE_NO}; // 定义参数列表的数组
				CMessageFormat::Format(s.msg,_RES("QM00S0004397")/*制造命令号[{0}]试样号[{1}]对应的QV钢水样不存在。*/, arguments, 2); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts24.ST_SAMPLE_DIV == "4" && tqmts24.GAS_TYPE_DIV == "1")//ON样
			{
				fetchRowCount = 0;
				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TQMTS25 "
						"  WHERE PONO = @tqmts24.PONO "
						"    AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO "
						"    AND ELM_CODE IN ('014','016') "
						"  ORDER BY ELM_CODE ASC ";
					break;
				}
				cmd_tqmts25.SetCommandText(sqlstr);				
				cmd_tqmts25.Parameters.Set("tqmts24.PONO", tqmts24.PONO);
				cmd_tqmts25.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24.ST_SAMPLE_NO);
				cmd_tqmts25.ExecuteReader();
				while (cmd_tqmts25.Read())
				{
					cmd_tqmts25.Fetch(tqmts25);

					fetchRowCount++;

					tqmts29.REC_CREATOR     = s.userid;
					tqmts29.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts29.REC_REVISOR     = " ";
					tqmts29.REC_REVISE_TIME = " ";
					tqmts29.ARCHIVE_FLAG    = " ";
					tqmts29.PONO            = tqmts25.PONO;
					tqmts29.HEAT_NO         = tqmts25.HEAT_NO;
					tqmts29.ST_NO           = tqmts25.ST_NO;
					tqmts29.ELM_CODE        = tqmts25.ELM_CODE;
					tqmts29.ELM_NAME        = tqmts25.ELM_NAME;
					tqmts29.ELM_VALUE       = tqmts25.ELM_ACT;
					tqmts29.ELM_OK          = tqmts25.ELM_OK;
					tqmts29.ST_SAMPLE_TYPE  = tqmts25.ST_SAMPLE_TYPE;
					tqmts29.ELM_BACKUP      = tqmts25.ELM_BACKUP;
					tqmts29.FACTORY_DIV     = tqmts24.FACTORY_DIV;

					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT CODE_DESC_2_CONTENT "
							"   FROM TEP0002 "
							"  WHERE CODE_CLASS = 'QMYS' "
							"    AND CODE = @tqmts29.ELM_CODE ";
						break;
					}
					cmd_tep0002.SetCommandText(sqlstr);
					cmd_tep0002.Parameters.Set("tqmts29.ELM_CODE", tqmts29.ELM_CODE);
					cmd_tep0002.ExecuteReader();
					if (cmd_tep0002.Read())
					{
						tqmts29.ELM_POS = CDecimal::Parse(cmd_tep0002.GetString(1));
					}
					cmd_tep0002.Close();

					tqmts29.ELM_UNIT        = "%";

					tqmts29.Delete("HEAT_NO, ELM_CODE"); //条件字段项
					tqmts29.Insert();
				}
				cmd_tqmts25.Close();
			}

			if( tqmts24.ST_SAMPLE_DIV == "4" && tqmts24.GAS_TYPE_DIV == "2")//H样
			{
				fetchRowCount = 0;
				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TQMTS25 "
						"  WHERE PONO = @tqmts24.PONO "
						"    AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO "
						"    AND ELM_CODE IN ('001') "
						"  ORDER BY ELM_CODE ASC ";
					break;
				}
				cmd_tqmts25.SetCommandText(sqlstr);				
				cmd_tqmts25.Parameters.Set("tqmts24.PONO", tqmts24.PONO);
				cmd_tqmts25.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24.ST_SAMPLE_NO);
				cmd_tqmts25.ExecuteReader();
				while (cmd_tqmts25.Read())
				{
					cmd_tqmts25.Fetch(tqmts25);

					fetchRowCount++;

					tqmts29.REC_CREATOR     = s.userid;
					tqmts29.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts29.REC_REVISOR     = " ";
					tqmts29.REC_REVISE_TIME = " ";
					tqmts29.ARCHIVE_FLAG    = " ";
					tqmts29.PONO            = tqmts25.PONO;
					tqmts29.HEAT_NO         = tqmts25.HEAT_NO;
					tqmts29.ST_NO           = tqmts25.ST_NO;
					tqmts29.ELM_CODE        = tqmts25.ELM_CODE;
					tqmts29.ELM_NAME        = tqmts25.ELM_NAME;
					tqmts29.ELM_VALUE       = tqmts25.ELM_ACT;
					tqmts29.ELM_OK          = tqmts25.ELM_OK;
					tqmts29.ST_SAMPLE_TYPE  = tqmts25.ST_SAMPLE_TYPE;
					tqmts29.ELM_BACKUP      = tqmts25.ELM_BACKUP;
					tqmts29.FACTORY_DIV     = tqmts24.FACTORY_DIV;

					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT CODE_DESC_2_CONTENT "
							"   FROM TEP0002 "
							"  WHERE CODE_CLASS = 'QMYS' "
							"    AND CODE = @tqmts29.ELM_CODE ";
						break;
					}
					cmd_tep0002.SetCommandText(sqlstr);
					cmd_tep0002.Parameters.Set("tqmts29.ELM_CODE", tqmts29.ELM_CODE);
					cmd_tep0002.ExecuteReader();
					if (cmd_tep0002.Read())
					{
						tqmts29.ELM_POS = CDecimal::Parse(cmd_tep0002.GetString(1));
					}
					cmd_tep0002.Close();

					tqmts29.ELM_UNIT        = "%";

					tqmts29.Delete("HEAT_NO, ELM_CODE"); //条件字段项
					tqmts29.Insert();
				}
				cmd_tqmts25.Close();
			}
		}//for
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

