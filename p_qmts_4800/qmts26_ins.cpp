/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-09
Description: 渣样实绩后备新增
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中





/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///渣样实绩后备新增
/// <para>
/// 1.渣样实绩后备新增
/// 
/// </para>
/// <para>数据库表：TQMTS26(实绩_渣样成分)				</para>
/// <para>主调用函数：前台QMTS26画面的F3(新增)调用		</para>
/// <para>需调用函数：									/para>
/// </summary>
/// <param name="heat_no">熔炼号				</param>
/// <param name="st_sample_no">试样号			</param>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts26_ins)


int f_qmts26_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal v_count = 0;
	CString s_station_no = "";
	CString s_station_name = "";
	int elm_num = 0;//实绩元素数量

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts26("TQMTS26");
	CModel tep0002("TEP0002");
	CModel tqmts24("TQMTS24");
	CModel tqmts0x("TQMTS0X");
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
			//s_station_name = bcls_rec->Tables[0].Rows[i]["STATION_NAME"].ToString().TrimOrBlank();

			Log::Trace("", "","qmts26_ins IN:---heat_no[{0}]",(const char*)tqmts24["HEAT_NO"].ToString());
			//Log::Trace("", "","qmts26_ins IN:---analyse_time[{0}]",(const char*)tqmts24["ANALYSE_TIME"].ToString());
			Log::Trace("", "","qmts26_ins IN:---sample_ifgood[{0}]",(const char*)tqmts24["SAMPLE_IFGOOD"].ToString());
			//Log::Trace("", "","qmts26_ins IN:---s_station_name[{0}]",(const char*)s_station_name);

			//校验传入参数
			if(tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if(s_station_name.TrimOrBlank() == " ")
			//{
			//	strcpy(s.msg,"工位(设备)名称不允许为空。");  
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			tqmts0x["ST_NO"] = tqmts24["ST_NO"];
			tqmts0x.Query("ST_NO");
			//if(tqmts24["ANALYSE_TIME"].ToString().TrimOrBlank() == " ")
			//{
			//	strcpy(s.msg,_RES("QM00S0004335")/*分析时间不允许为空。*/); 
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//if(tqmts24["SAMPLE_IFGOOD"].ToString().TrimOrBlank() == " ")
			//{
			//	strcpy(s.msg,"试样良否不允许为空!"); 
				//throw CApplicationException(-1, s.msg, log.Location);
			//}

			tqmts24["REC_CREATOR"] = s.userid;
			tqmts24["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts24["REC_REVISOR"] = " ";
			tqmts24["REC_REVISE_TIME"] = " ";
			tqmts24["ARCHIVE_FLAG"] = " ";
			tqmts24["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];

			tqmts24["ST_SAMPLE_DIV"] = "3"; //炼钢试样区分 1-钢样;2-铁样;3-渣样;4-气体样
			tqmts24["GAS_TYPE_DIV"] = " ";
			tqmts24["JUDGE_CODE"] = " ";
			tqmts24["REP_ELM_SEL_FLAG"] = " ";
			tqmts24["ELM_TYPE_DIV"] = " ";
			tqmts24["ANALYSE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT PONO,SM_PLAN_NO,ST_NO,FIN_ST_NO "
							 "  FROM TQMTS23 "
							 " WHERE HEAT_NO = @heat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts24["PONO"] = cmd_inq.GetString(1);
				tqmts24["SM_PLAN_NO"] = cmd_inq.GetString(2);
				tqmts24["ST_NO"] = cmd_inq.GetString(3);
				tqmts24["FIN_ST_NO"] = cmd_inq.GetString(4);
			}
			else
			{
				sprintf(s.msg, "熔炼号[%s]不存在！",(const char*)tqmts24["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();
			Log::Trace("", "","sm_plan_no[{0}]",(const char*)tqmts24["SM_PLAN_NO"].ToString());

			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT STATION_ID,STATION_NO "
			//				 "  FROM TPSSMD1 "
			//				 " WHERE STATION_NAME = @station_name ";
			//		break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("station_name", s_station_name.Trim());
			//cmd_inq.ExecuteReader();
			//if (cmd_inq.Read())
			//{
			//	tqmts24["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(1);
			//	s_station_no = cmd_inq.GetString(2);
			//}
			//cmd_inq.Close();
			//Log::Trace("", "","STATION_NO = [{0}]",(const char*)s_station_no);
			//Log::Trace("", "","WHOLE_BACKLOG_CODE = [{0}]",(const char*)tqmts24["WHOLE_BACKLOG_CODE"].ToString());

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT MAX(ST_SAMPLE_SEQ) "
							 "  FROM TQMTS24 "
							 " WHERE  "
							 "     HEAT_NO = @heat_no "
							 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
							 "   AND ST_SAMPLE_DIV = @st_sample_div ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("whole_backlog_code", tqmts24["WHOLE_BACKLOG_CODE"].ToString().Trim());
			cmd_inq.Parameters.Set("st_sample_div", tqmts24["ST_SAMPLE_DIV"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts24["ST_SAMPLE_SEQ"] = cmd_inq.GetDecimal(1)+1;
			}
			else
			{
				tqmts24["ST_SAMPLE_SEQ"] = 1;
			}
			cmd_inq.Close();
			Log::Trace("", "","st_sample_seq[{0}]",tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToInt32());

			//拼试样号——炼钢厂(1)+炼钢试样区分(1)+工序(1)+工位(1)+炼钢试样顺序号(1)+气体类型区分(1)  非气体样：气体类型区分写死"Z"
			tqmts24["ST_SAMPLE_NO"] = "A" + tqmts24["ST_SAMPLE_DIV"].ToString().Trim() + tqmts24["WHOLE_BACKLOG_CODE"].ToString() + s_station_no.Trim() + tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToString().Trim() + "Z";
			Log::Trace("", "","ST_SAMPLE_NO[{0}]",(const char*)tqmts24["ST_SAMPLE_NO"].ToString());

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT count(*) "
							 "  FROM TQMTS24 "
							 " WHERE HEAT_NO = @heat_no "
							 "   AND ST_SAMPLE_NO = @st_sample_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("st_sample_no", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			v_count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			if(v_count > 0)
			{
				sprintf(s.msg,"熔炼号[%s]试样号[%s]的渣样实绩已存在，不可新增，只可修改。",(const char*)tqmts24["HEAT_NO"].ToString(),(const char*)tqmts24["ST_SAMPLE_NO"].ToString());  
				throw CApplicationException(-1, s.msg, log.Location);
			}

			Log::Trace("", "","INSERT TQMTS26");
			tqmts26["REC_CREATOR"]			= tqmts24["REC_CREATOR"];
			tqmts26["REC_CREATE_TIME"]		= tqmts24["REC_CREATE_TIME"];
			tqmts26["REC_REVISOR"]			= tqmts24["REC_REVISOR"];
			tqmts26["REC_REVISE_TIME"]		= tqmts24["REC_REVISE_TIME"];
			tqmts26["ARCHIVE_FLAG"]		= tqmts24["ARCHIVE_FLAG"];
			tqmts26["PONO"]				= tqmts24["PONO"];
			tqmts26["HEAT_NO"]				= tqmts24["HEAT_NO"];
			tqmts26["ST_SAMPLE_NO"]		= tqmts24["ST_SAMPLE_NO"];
			tqmts26["ST_SAMPLE_NAME"]		= " ";
			tqmts26["ST_NO"]				= tqmts24["ST_NO"];
			tqmts26["WHOLE_BACKLOG_CODE"]	= tqmts24["WHOLE_BACKLOG_CODE"];
			Log::Trace("", "", "INSERT TQMTS26-2");
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TEP0002 "
							 " WHERE CODE_CLASS = 'QMZS' "
							 "   AND TRIM(CODE_DESC_2_CONTENT) is not null "
							 " ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tep0002); 
				tqmts26["ELM_ACT"] = bcls_rec->Tables[0].Rows[i]["ELM_ACT"].ToString().TrimOrBlank(); //根据元素代码得到前台传入的元素值
				Log::Trace("", "","ELM_CODE[{0}], ELM_NAME[{1}], ELM_ACT[{2}]",(const char*)tqmts26["ELM_CODE"].ToString(),(const char*)tqmts26["ELM_NAME"].ToString(),tqmts26["ELM_ACT"].ToDecimal().ToDouble());
				tqmts26["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
				tqmts26["ELM_CODE"] = tep0002["CODE"];
				if (tqmts26["ELM_CODE"].ToString() == bcls_rec->Tables[0].Rows[i]["ELM_CODE"].ToString())
				{
					elm_num++;
					tqmts26.TrimOrBlank();
					tqmts26.Insert();
				}
			}
			cmd_inq.Close();
	 
			if(elm_num<1)
			{
				strcpy(s.msg,_RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		
			tqmts24.TrimOrBlank();
			tqmts24.Insert();
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
