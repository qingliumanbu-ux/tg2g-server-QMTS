/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-09
Description: 渣样实绩查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中

 
 

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///渣样实绩查询
/// <para>
/// 1.渣样实绩查询
/// 
/// </para>
/// <para>数据库表：TQMTS26(实绩_渣样成分)				</para>
/// <para>主调用函数：前台QMTS26画面的F2(查询)调用		</para>
/// <para>需调用函数：									</para>
/// </summary>
/// <param name="heat_no">熔炼号				</param>
/// <param name="st_sample_no">试样号			</param>
/// <param name="st_no">出钢记号				</param>
/// <returns>  </returns>
===========================================================</remark>*/


// service入口
BM2F_ENTERACE(qmts26_inq)


int f_qmts26_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int q_flag = 0;
	CString h5_flag = "";
	CString q_heat_no = " ";
	CString q_st_no = " ";
	CString q_analyse_time_from = " ";
    CString q_analyse_time_to = " ";
	int i_every_page = 50;
	int i_now_record = 1;
	CDecimal row_id = 0;
	CDecimal i_total_count = 0;
	CString s_station_no = "";
	CString s_station_name = "";

	CString sqlstr = "";
	
	/* 实体类定义 */
	CModel tqmts26("TQMTS26");
	CModel tep0002("TEP0002");
	CModel tqmts24("TQMTS24");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("H5_FLAG"))
		{
			h5_flag = bcls_rec->Tables[0].Rows[0]["H5_FLAG"].ToString().Trim();
		}
		Log::Trace("", "", "h5_flag = {0}", h5_flag);
		q_flag = bcls_rec->Tables[1].Rows[0]["flag"].ToDecimal().ToInt16();
		q_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_analyse_time_from = bcls_rec->Tables[0].Rows[0]["analyse_time_from"].ToString().Trim();
		q_analyse_time_to = bcls_rec->Tables[0].Rows[0]["analyse_time_to"].ToString().Trim();
		if (h5_flag.Trim() != "") 
		{
			i_now_record = bcls_rec->Tables["PageInfo"].Rows[0]["RecordFrom"].ToDecimal().ToInt32();
			i_every_page = bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"].ToDecimal().ToInt32();
		}
		else
		{
			i_every_page = bcls_rec->Tables[1].Rows[0]["every_page"].ToDecimal().ToInt32();/*每页显示的记录数*/
			i_now_record = bcls_rec->Tables[1].Rows[0]["now_record"].ToDecimal().ToInt32();/*开始记录序号 i_every_page*PageFrom*/

		}

		Log::Trace("", "","qmts26_inq IN:---q_flag = [{0}]",q_flag);
		Log::Trace("", "","qmts26_inq IN:---q_heat_no = [{0}]",(const char*)q_heat_no);
		Log::Trace("", "","qmts26_inq IN:---q_st_no = [{0}]",(const char*)q_st_no);
		Log::Trace("", "","qmts26_inq IN:---q_analyse_time_from[{0}], q_analyse_time_to[{1}]", (const char*)q_analyse_time_from, (const char*)q_analyse_time_to);
		Log::Trace("", "","qmts26_inq IN:---i_every_page[{0}], i_now_record[{1}]",i_every_page,i_now_record);

		//设置输出块列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"HEAT_NO");
		bcls_ret->Tables[0].Columns["HEAT_NO"].set_Caption(_RES("QM00S0004301")/*熔炼号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"PONO");
		bcls_ret->Tables[0].Columns["PONO"].set_Caption(_RES("QM00S0004303")/*制造命令号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ST_SAMPLE_NO");
		bcls_ret->Tables[0].Columns["ST_SAMPLE_NO"].set_Caption(_RES("QM00S0004304")/*试样号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ST_NO");
		//bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("出钢记号");
		bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("内部钢种");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"STATION_NAME");
		bcls_ret->Tables[0].Columns["STATION_NAME"].set_Caption("工位");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_ACT");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_NAME");
		/****************************************************************/
		/*******************根据代码配置表压入列名***********************/
		/****************************************************************/
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
						 "   AND TRIM(CODE_DESC_2_CONTENT) IS NOT null "
						 " ORDER BY CODE_DESC_2_CONTENT ASC ";
				break;
		}
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		while (cmd_inq1.Read())
		{
			cmd_inq1.Fetch(tep0002);
			Log::Trace("", "","tep0002.CODE[{0}], tep0002.CODE_DESC_1_CONTENT[{1}]",(const char*)tep0002["CODE"].ToString(),(const char*)tep0002["CODE_DESC_1_CONTENT"].ToString());
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL,tep0002["CODE"].ToString());  //压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString()].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString());//压入列标题(元素名)
		}
		cmd_inq1.Close();
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"SM_PLAN_NO");
		bcls_ret->Tables[0].Columns["SM_PLAN_NO"].set_Caption(_RES("QM00S0004308")/*炼钢计划号*/);
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME,"ANALYSE_TIME");
		bcls_ret->Tables[0].Columns["ANALYSE_TIME"].set_Caption(_RES("QM00S0004307")/*分析时刻*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"SAMPLE_IFGOOD");
		bcls_ret->Tables[0].Columns["SAMPLE_IFGOOD"].set_Caption(_RES("QM00S0004333")/*试样良否*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"REC_CREATOR");
		bcls_ret->Tables[0].Columns["REC_CREATOR"].set_Caption(_RES("QM00S0004192")/*记录创建责任者*/);
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME,"REC_CREATE_TIME");
		bcls_ret->Tables[0].Columns["REC_CREATE_TIME"].set_Caption(_RES("QM00S0004193")/*记录创建时刻*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"REC_REVISOR");
		bcls_ret->Tables[0].Columns["REC_REVISOR"].set_Caption(_RES("QM00S0004194")/*记录修改责任者*/);
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME,"REC_REVISE_TIME");
		bcls_ret->Tables[0].Columns["REC_REVISE_TIME"].set_Caption(_RES("QM00S0004195")/*记录修改时刻*/);

		//flag=0,画面load元素信息用，不需取值，跳出
		if (q_flag == 0)
		{
			return doFlag;
		}

		Log::Trace("", "","SELECT TQMTS24...");
		/****************************************************************/
		/*******************从实绩表中读出元素值*************************/
		/****************************************************************/
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				Log::Trace("", "", "走DB2");
				sqlstr = " SELECT * "
					"   FROM (SELECT ROW_NUMBER() over() as ROW_ID,A.* "
					"           FROM (SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,SM_PLAN_NO,ANALYSE_TIME,SAMPLE_IFGOOD "
					"                   FROM TQMTS24 "
					"                  WHERE HEAT_NO LIKE @q_heat_no||'%' "
					"                    AND ST_NO LIKE '%'||@q_st_no||'%' "
					"                    AND ST_SAMPLE_DIV = '3' "
					;
				if (q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(ANALYSE_TIME,1,14) >= @q_analyse_time_from ";
				if (q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(ANALYSE_TIME,1,14) <= @q_analyse_time_to ";
				sqlstr = sqlstr + "                  ORDER BY HEAT_NO ASC, ST_SAMPLE_NO ASC "
					"                ) A "
					"        ) "
					"  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";
				break;
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				Log::Trace("", "", "走默认数据库语句");
				sqlstr = " SELECT * "
						 "   FROM (SELECT ROWNUM ROW_ID,A.* "
						 "           FROM (SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,SM_PLAN_NO,ANALYSE_TIME,SAMPLE_IFGOOD "
						 "                   FROM TQMTS24 "
						 "                  WHERE HEAT_NO LIKE @q_heat_no||'%' "
						 "                    AND ST_NO LIKE '%'||@q_st_no||'%' "
						 "                    AND ST_SAMPLE_DIV = '3' ";
				if ( q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(ANALYSE_TIME,1,14) >= @q_analyse_time_from ";
				if ( q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(ANALYSE_TIME,1,14) <= @q_analyse_time_to ";
				sqlstr = sqlstr + "                  ORDER BY HEAT_NO ASC, ST_SAMPLE_NO ASC "
						 "                ) A "
						 "        ) "
						 "  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";
						 
				break;
		}
		Log::Trace("", "", "1sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
		cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
		cmd_inq.Parameters.Set("q_analyse_time_from", q_analyse_time_from);
		cmd_inq.Parameters.Set("q_analyse_time_to", q_analyse_time_to);
		cmd_inq.Parameters.Set("i_now_record", i_now_record);
		cmd_inq.Parameters.Set("i_every_page", i_every_page);
		Log::Trace("", "", "q_analyse_time_from[{0}]q_analyse_time_to[{1}]", q_analyse_time_from,q_analyse_time_to);
		cmd_inq.ExecuteReader(); 
		Log::Trace("", "", "1============0");
		while(cmd_inq.Read())
		{
			Log::Trace("", "", "innnnnnnnn");
			row_id = cmd_inq.GetDecimal(1);
			tqmts24["FACTORY_DIV"] = cmd_inq.GetString(2);
			tqmts24["HEAT_NO"] = cmd_inq.GetString(3);
			tqmts24["PONO"] = cmd_inq.GetString(4);
			tqmts24["ST_SAMPLE_NO"] = cmd_inq.GetString(5);
			tqmts24["ST_NO"] = cmd_inq.GetString(6);
			tqmts24["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(7);
			tqmts24["SM_PLAN_NO"] = cmd_inq.GetString(8);
			tqmts24["ANALYSE_TIME"] = cmd_inq.GetString(9);
			tqmts24["SAMPLE_IFGOOD"] = cmd_inq.GetString(10);
	    
			Log::Trace("", "","tqmts24.HEAT_NO = [{0}]",(const char*)tqmts24["HEAT_NO"].ToString());
			Log::Trace("", "","tqmts24.ST_SAMPLE_NO = [{0}]",(const char*)tqmts24["ST_SAMPLE_NO"].ToString());
			//HYF 20130427 SubstringNE
			s_station_no = tqmts24["ST_SAMPLE_NO"].ToString().SubstringNE(3,1);

			Log::Trace("", "","FACTORY_DIV = [{0}]",(const char*)tqmts24["FACTORY_DIV"].ToString());
			Log::Trace("", "","STATION_ID = [{0}]",(const char*)tqmts24["WHOLE_BACKLOG_CODE"].ToString());
			Log::Trace("", "","STATION_NO = [{0}]",(const char*)s_station_no);

			s_station_name = "";
			//switch(conn->DatabaseKind)    //临时注释 23/8/23
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT STATION_NAME "
			//				 "  FROM TPSSMD1 "
			//				 " WHERE FACTORY_DIV = @tqmts24.FACTORY_DIV "
			//				 "   AND STATION_ID = @tqmts24.WHOLE_BACKLOG_CODE "
			//				 "   AND STATION_NO = @s_station_no ";
			//		break;
			//}
			//cmd_inq_01.SetCommandText(sqlstr);
			//cmd_inq_01.Parameters.Set("tqmts24.FACTORY_DIV", tqmts24["FACTORY_DIV"].ToString().Trim());
			//cmd_inq_01.Parameters.Set("tqmts24.WHOLE_BACKLOG_CODE", tqmts24["WHOLE_BACKLOG_CODE"].ToString().Trim());
			//cmd_inq_01.Parameters.Set("s_station_no", s_station_no.Trim());
			//cmd_inq_01.ExecuteReader();
			//if (cmd_inq_01.Read())
			//{
			//	s_station_name = cmd_inq_01.GetString(1);
			//}
			//cmd_inq_01.Close();
			//Log::Trace("", "","STATION_NAME = [{0}]",(const char*)s_station_name);

			//当页的记录返回前台
			CDataRow& row = bcls_ret->Tables[0].Rows.Add();
			row["HEAT_NO"] = tqmts24["HEAT_NO"];
			row["PONO"] = tqmts24["PONO"];
			row["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			row["ST_NO"] = tqmts24["ST_NO"];
			row["STATION_NAME"] = s_station_name;
			row["SM_PLAN_NO"] = tqmts24["SM_PLAN_NO"];
			row["ANALYSE_TIME"] = CDateTime::Parse(tqmts24["ANALYSE_TIME"].ToString());
			row["SAMPLE_IFGOOD"] = tqmts24["SAMPLE_IFGOOD"];

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TQMTS26 "
							 " WHERE HEAT_NO = @tqmts24.HEAT_NO "
							 "   AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO "
							 " ORDER BY ELM_CODE ASC ";
					break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("tqmts24.HEAT_NO", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq_01.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			while (cmd_inq_01.Read())
			{
				cmd_inq_01.Fetch(tqmts26);
				Log::Trace("", "","tqmts26.ELM_CODE[{0}], tqmts26.ELM_ACT[{1}]",(const char*)tqmts26["ELM_CODE"].ToString(),tqmts26["ELM_ACT"].ToDecimal().ToDouble());
				row["ELM_CODE"] = tqmts26["ELM_CODE"].ToString();
				row["WHOLE_BACKLOG_CODE"] = tqmts26["WHOLE_BACKLOG_CODE"].ToString();
				row["ELM_NAME"] = tqmts26["ELM_NAME"].ToString();
				row["ELM_ACT"] = tqmts26["ELM_ACT"].ToString();
			}
			cmd_inq_01.Close();

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME "
							 "  FROM TQMTS24 "
							 " WHERE HEAT_NO = @tqmts24.HEAT_NO "
							 "   AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO ";
					break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("tqmts24.HEAT_NO", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq_01.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			if (cmd_inq_01.Read())
			{
				tqmts24["REC_CREATOR"] = cmd_inq_01.GetString(1);
				tqmts24["REC_CREATE_TIME"] = cmd_inq_01.GetString(2);
				tqmts24["REC_REVISOR"] = cmd_inq_01.GetString(3);
				tqmts24["REC_REVISE_TIME"] = cmd_inq_01.GetString(4);
			}
			cmd_inq_01.Close();
			Log::Trace("", "","tqmts24.REC_CREATE_TIME = [{0}]",(const char*)tqmts24["REC_CREATE_TIME"].ToString());
			row["REC_CREATOR"] = tqmts24["REC_CREATOR"];
			if(tqmts24["REC_CREATE_TIME"].ToString().TrimOrBlank() != " ")
				row["REC_CREATE_TIME"] = CDateTime::Parse(tqmts24["REC_CREATE_TIME"].ToString());
			row["REC_REVISOR"] = tqmts24["REC_REVISOR"];
			if(tqmts24["REC_REVISE_TIME"].ToString().TrimOrBlank() != " ")
				row["REC_REVISE_TIME"] = CDateTime::Parse(tqmts24["REC_REVISE_TIME"].ToString());
		}
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL,"TOTAL_COUNT");
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1) "
						 "  FROM (SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,SM_PLAN_NO,ANALYSE_TIME,SAMPLE_IFGOOD "
						 "          FROM TQMTS24 "
						 "         WHERE HEAT_NO LIKE @q_heat_no||'%' "
						 "           AND ST_NO LIKE '%'||@q_st_no||'%' "
						 "           AND ST_SAMPLE_DIV = '3' "
						 ;
				if ( q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(ANALYSE_TIME,1,14) >= @q_analyse_time_from ";
				if ( q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(ANALYSE_TIME,1,14) <= @q_analyse_time_to ";
				sqlstr = sqlstr + "       ) ";
				break;
		}
		Log::Trace("", "", "记录总数sql = [{0}]", sqlstr);
		cmd_inq2.SetCommandText(sqlstr);
		cmd_inq2.Parameters.Set("q_heat_no", q_heat_no.Trim());
		cmd_inq2.Parameters.Set("q_st_no", q_st_no.Trim());
		cmd_inq2.Parameters.Set("q_analyse_time_from", q_analyse_time_from.Trim());
		cmd_inq2.Parameters.Set("q_analyse_time_to", q_analyse_time_to.Trim());
	//	i_total_count = cmd_inq.ExecuteScalar();   //
		i_total_count = cmd_inq2.ExecuteScalar();
		Log::Trace("", "", "i_total_count = [{0}]", i_total_count);
		cmd_inq2.Close();
		CDataRow& row = bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[0]["TOTAL_COUNT"] = i_total_count;
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
