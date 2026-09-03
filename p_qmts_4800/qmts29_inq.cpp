/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-14
Description: 炉次代表成分查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次代表成分查询
/// <para>
/// 1.炉次代表成分查询
/// 
/// </para>
/// <para>数据库表：TQMTS29(实绩_炉次代表成分)			</para>
/// <para>主调用函数：前台QMTS29画面的F2(查询)调用		</para>
/// <para>需调用函数：							</para>
/// </summary>
/// <param name="HEAT_NO">  熔炼号				</param>
/// <param name="ST_NO">  出钢记号				</param>
/// <returns>  </returns>
===========================================================</remark>*/


// service入口
BM2F_ENTERACE(qmts29_inq)


int f_qmts29_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int q_flag = 0;
	CString q_heat_no = " ";
	CString q_st_no = " ";
	CString q_rec_create_time_from = " ";
	CString q_rec_create_time_to = " ";
	int i_every_page = 50;
	int i_now_record = 1;
	CDecimal row_id = 0;
	CDecimal i_total_count = 0;
	CString s_factory_div = "";  //2023.1.16

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts29("TQMTS29");
	CModel tep0002("TEP0002");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	try
	{
		q_flag = bcls_rec->Tables[0].Rows[0]["flag"].ToDecimal().ToInt16();
		q_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_rec_create_time_from = bcls_rec->Tables[0].Rows[0]["rec_create_time_from"].ToString().Trim();
		q_rec_create_time_to = bcls_rec->Tables[0].Rows[0]["rec_create_time_to"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV")) s_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();

		i_every_page = bcls_rec->Tables[0].Rows[0]["every_page"].ToDecimal().ToInt32();/*每页显示的记录数*/
		i_now_record = bcls_rec->Tables[0].Rows[0]["now_record"].ToDecimal().ToInt32();/*开始记录序号 i_every_page*PageFrom*/

		Log::Trace("", "", "qmts25_inq IN:---q_flag = [{0}]", q_flag);
		Log::Trace("", "", "qmts25_inq IN:---q_heat_no = [{0}]", (const char*)q_heat_no);
		Log::Trace("", "", "qmts25_inq IN:---q_st_no = [{0}]", (const char*)q_st_no);
		Log::Trace("", "", "qmts25_inq IN:---FACTORY_DIV = [{0}]", (const char*)s_factory_div);
		Log::Trace("", "", "qmts25_inq IN:---q_rec_create_time_from[{0}], q_rec_create_time_to[{1}]", (const char*)q_rec_create_time_from, (const char*)q_rec_create_time_to);
		Log::Trace("", "", "qmts25_inq IN:---i_every_page[{0}], i_now_record[{1}]", i_every_page, i_now_record);

		//设置输出块列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_ret->Tables[0].Columns["HEAT_NO"].set_Caption(_RES("QM00S0004301")/*熔炼号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PONO");
		bcls_ret->Tables[0].Columns["PONO"].set_Caption(_RES("QM00S0004303")/*制造命令号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		//bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("出钢记号");
		bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("内部钢种");

		/****************************************************************/
		/*******************根据代码配置表压入列名***********************/
		/****************************************************************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TEP0002 "
				" WHERE CODE_CLASS = 'QMYS' "
				"   AND TRIM(CODE_DESC_2_CONTENT) IS NOT null "
				" ORDER BY CODE_DESC_2_CONTENT ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tep0002);
			Log::Trace("", "", "tep0002.CODE[{0}], tep0002.CODE_DESC_1_CONTENT[{1}]", (const char*)tep0002["CODE"].ToString(), (const char*)tep0002["CODE_DESC_1_CONTENT"].ToString());
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString());  //压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString()].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString());//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString() + "_OK");
		}
		cmd_inq.Close();
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_CREATOR");
		bcls_ret->Tables[0].Columns["REC_CREATOR"].set_Caption(_RES("QM00S0004192")/*记录创建责任者*/);
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "REC_CREATE_TIME");
		bcls_ret->Tables[0].Columns["REC_CREATE_TIME"].set_Caption(_RES("QM00S0004193")/*记录创建时刻*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_REVISOR");
		bcls_ret->Tables[0].Columns["REC_REVISOR"].set_Caption(_RES("QM00S0004194")/*记录修改责任者*/);
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "REC_REVISE_TIME");
		bcls_ret->Tables[0].Columns["REC_REVISE_TIME"].set_Caption(_RES("QM00S0004195")/*记录修改时刻*/);

		//flag=0,画面load元素信息用，不需取值，跳出
		if (q_flag == 0)
		{
			return doFlag;
		}

		Log::Trace("", "", "SELECT TQMTS29");
		/****************************************************************/
		/*******************从实绩表中读出元素值*************************/
		/****************************************************************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//sqlstr = " SELECT * "
			//	"   FROM (SELECT ROW_NUMBER() over() as ROW_ID,A.* "
			//	"           FROM (SELECT DISTINCT HEAT_NO,PONO,ST_NO "
			//	"                   FROM TQMTS29 "
			//	"                  WHERE HEAT_NO LIKE @q_heat_no||'%' "
			//	"                    AND ST_NO LIKE '%'||@q_st_no||'%' ";
			////if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = '" + s_factory_div + "' ";
			//if (q_rec_create_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(REC_CREATE_TIME,0,8) >= @q_rec_create_time_from ";
			//if (q_rec_create_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(REC_CREATE_TIME,0,8) <= @q_rec_create_time_to ";
			//sqlstr = sqlstr + "                ORDER BY rec_create_time DESC ,pono ASC  "
			//	"                ) A "
			//	"        ) "
			//	"  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";
			//break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"FROM (SELECT ROW_NUMBER() over() as ROW_ID,A.* "
				"FROM (SELECT DISTINCT HEAT_NO,PONO,ST_NO,REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME "
				"FROM TQMTS29 "
				"WHERE HEAT_NO LIKE @q_heat_no||'%' "
				"AND ST_NO LIKE '%'||@q_st_no||'%' ";
			if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = '" + s_factory_div + "' ";
			if (q_rec_create_time_from.Trim() != "")  sqlstr = sqlstr + " AND REC_CREATE_TIME >= @q_rec_create_time_from ";
			if (q_rec_create_time_to.Trim() != "")  sqlstr = sqlstr + " AND REC_CREATE_TIME <= @q_rec_create_time_to ";
			sqlstr = sqlstr + "  ORDER BY rec_create_time DESC  ) A  ) ";
			sqlstr = sqlstr + "  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";
			break;
		}
		Log::Trace("", "", "sql:{0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
		cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
		cmd_inq.Parameters.Set("q_rec_create_time_from", q_rec_create_time_from.Trim());
		cmd_inq.Parameters.Set("q_rec_create_time_to", q_rec_create_time_to.Trim());
		cmd_inq.Parameters.Set("i_now_record", i_now_record);
		cmd_inq.Parameters.Set("i_every_page", i_every_page);
		cmd_inq.ExecuteReader();
		Log::Trace("", "", "sql:{0}", sqlstr);
		while (cmd_inq.Read())
		{
			row_id = cmd_inq.GetDecimal(1);
			tqmts29["HEAT_NO"] = cmd_inq.GetString(2);
			tqmts29["PONO"] = cmd_inq.GetString(3);
			tqmts29["ST_NO"] = cmd_inq.GetString(4);
			tqmts29["REC_CREATOR"] = cmd_inq.GetString(5);
			tqmts29["REC_CREATE_TIME"] = cmd_inq.GetString(6);
			tqmts29["REC_REVISOR"] = cmd_inq.GetString(7);
			tqmts29["REC_REVISE_TIME"] = cmd_inq.GetString(8);


			Log::Trace("", "", "tqmts29.HEAT_NO = [{0}]", (const char*)tqmts29["HEAT_NO"].ToString());
			Log::Trace("", "", "tqmts29.PONO = [{0}]", (const char*)tqmts29["PONO"].ToString());
			Log::Trace("", "", "tqmts29.REC_CREATE_TIME= [{0}]", (const char*)tqmts29["REC_CREATE_TIME"].ToString());

			//当页的记录返回前台
			CDataRow& row = bcls_ret->Tables[0].Rows.Add();
			row["HEAT_NO"] = tqmts29["HEAT_NO"];
			row["PONO"] = tqmts29["PONO"];
			row["ST_NO"] = tqmts29["ST_NO"];
			row["REC_CREATOR"] = tqmts29["REC_CREATOR"];
			if (tqmts29["REC_CREATE_TIME"].ToString().TrimOrBlank() != " ")
				row["REC_CREATE_TIME"] = CDateTime::Parse(tqmts29["REC_CREATE_TIME"].ToString());
			row["REC_REVISOR"] = tqmts29["REC_REVISOR"];
			if (tqmts29["REC_REVISE_TIME"].ToString().TrimOrBlank() != " ")
				row["REC_REVISE_TIME"] = CDateTime::Parse(tqmts29["REC_REVISE_TIME"].ToString());

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS29 "
					" WHERE PONO = @tqmts29.PONO "
					" ORDER BY ELM_CODE ASC ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("tqmts29.PONO", tqmts29["PONO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			while (cmd_inq_01.Read())
			{
				cmd_inq_01.Fetch(tqmts29);
				Log::Trace("", "", "tqmts29.ELM_CODE[{0}], tqmts29.ELM_VALUE[{1}], tqmts29.ELM_OK[{2}]", (const char*)tqmts29["ELM_CODE"].ToString(), tqmts29["ELM_VALUE"].ToDecimal().ToDouble(), tqmts29["ELM_OK"].ToDecimal().ToInt32());
				row[tqmts29["ELM_CODE"].ToString()] = tqmts29["ELM_VALUE"];
				row[tqmts29["ELM_CODE"].ToString() + "_OK"] = tqmts29["ELM_OK"];
			}
			cmd_inq_01.Close();

			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "SELECT REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME "
			//				 "  FROM TQMTS29 "
			//				 " WHERE PONO = @tqmts29.PONO ";
			//		break;
			//}
			//cmd_inq_01.SetCommandText(sqlstr);
			//cmd_inq_01.Parameters.Set("tqmts29.PONO", tqmts29["PONO"].ToString().Trim());
			//cmd_inq_01.ExecuteReader();
			//if (cmd_inq_01.Read())
			//{
			//	tqmts29["REC_CREATOR"] = cmd_inq_01.GetString(1);
			//	tqmts29["REC_CREATE_TIME"] = cmd_inq_01.GetString(2);
			//	tqmts29["REC_REVISOR"] = cmd_inq_01.GetString(3);
			//	tqmts29["REC_REVISE_TIME"] = cmd_inq_01.GetString(4);
			//}
			//cmd_inq_01.Close();
			//Log::Trace("", "","tqmts29.REC_CREATE_TIME= [{0}]",(const char*)tqmts29["REC_CREATE_TIME"].ToString());
			//row["REC_CREATOR"] = tqmts29["REC_CREATOR"];
			//if(tqmts29["REC_CREATE_TIME"].ToString().TrimOrBlank() != " ")
			//	row["REC_CREATE_TIME"] = CDateTime::Parse(tqmts29["REC_CREATE_TIME"].ToString());
			//row["REC_REVISOR"] = tqmts29["REC_REVISOR"];
			//if(tqmts29["REC_REVISE_TIME"].ToString().TrimOrBlank() != " ")
			//	row["REC_REVISE_TIME"] = CDateTime::Parse(tqmts29["REC_REVISE_TIME"].ToString());
		}
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "TOTAL_COUNT");
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT COUNT(1) "
				"  FROM (SELECT DISTINCT HEAT_NO,PONO,ST_NO "
				"          FROM TQMTS29 "
				"         WHERE HEAT_NO LIKE @q_heat_no||'%' "
				"           AND ST_NO LIKE '%'||@q_st_no||'%' ";
			if (q_rec_create_time_from.Trim() != "")  sqlstr = sqlstr + " AND REC_CREATE_TIME >= @q_rec_create_time_from ";
			if (q_rec_create_time_to.Trim() != "")  sqlstr = sqlstr + " AND REC_CREATE_TIME <= @q_rec_create_time_to ";
			sqlstr = sqlstr + "       ) ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
		cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
		cmd_inq.Parameters.Set("q_rec_create_time_from", q_rec_create_time_from.Trim());
		cmd_inq.Parameters.Set("q_rec_create_time_to", q_rec_create_time_to.Trim());
		i_total_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
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
