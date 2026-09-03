/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-12
Description: 工序成分标准查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 工序成分标准查询
/// <para>
/// 获取输入参数：TQMTS9A(成分标准) ；
/// </para>
/// <para>数据库表：TQMTS9A(成分标准));
///  前台画面QMTS9A成分标准F2(查询)调用  </para>
/// </summary>
/// <param name="TQMTS9A">成分标准    </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts9a_inq)

int f_qmts9a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int q_flag = 0;
	//CString base_code = "";
	CString q_steel_group = " ";
	//CString q_st_no = " ";
	//CString q_whole_backlog_code = " ";
	int i_every_page = 50;
	int i_now_record = 1;
	CDecimal row_id = 0;
	CDecimal i_total_count = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts9a("TQMTS9A");
	CModel tep0002("TEP0002");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	try
	{
		//q_flag = bcls_rec->Tables[0].Rows[0]["flag"].ToDecimal().ToInt16();
		q_steel_group = bcls_rec->Tables[0].Rows[0]["steel_group"].ToString().Trim();
		//q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		//q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		i_every_page = bcls_rec->Tables[0].Rows[0]["every_page"].ToDecimal().ToInt32();/*每页显示的记录数*/
		i_now_record = bcls_rec->Tables[0].Rows[0]["now_record"].ToDecimal().ToInt32();/*开始记录序号 i_every_page*PageFrom*/
		//base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();

		//Log::Trace("", "", "qmts9a_inq IN:---q_flag = [{0}]", q_flag);
		Log::Trace("", "", "qmts9a_inq IN:---q_steel_group = [{0}]", (const char*)q_steel_group);
		//Log::Trace("", "", "qmts9a_inq IN:---q_st_no = [{0}]", (const char*)q_st_no);
		//Log::Trace("", "", "qmts9a_inq IN:---q_whole_backlog_code = [{0}]", (const char*)q_whole_backlog_code);
		Log::Trace("", "", "qmts9a_inq IN:---i_every_page[{0}], i_now_record[{1}]", i_every_page, i_now_record);

		//设置输出块列名
		//bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		//bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("出钢记号");
		//bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("内部钢种");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STEEL_GROUP");
		bcls_ret->Tables[0].Columns["STEEL_GROUP"].set_Caption("钢种组");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
		//bcls_ret->Tables[0].Columns["WHOLE_BACKLOG_CODE"].set_Caption("工序");
		//bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "WHOLE_BACKLOG_SEQ");
		//bcls_ret->Tables[0].Columns["WHOLE_BACKLOG_SEQ"].set_Caption("工序顺序");
		/****************************************************************/
		/*******************根据代码配置表压入列名***********************/
		/****************************************************************/
		/*switch (conn->DatabaseKind)
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

			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString() + "_MAIN_MIN");     //压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString() + "_MAIN_MIN"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "主试最小");//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString() + "_MAIN_MAX");     //压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString() + "_MAIN_MAX"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "主试最大");//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString() + "_MAIN_AIM");     //压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString() + "_MAIN_AIM"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "主试目标");//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString() + "_SPE_MIN");		//压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString() + "_SPE_MIN"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "特采最小");//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tep0002["CODE"].ToString() + "_SPE_MAX");		//压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString() + "_SPE_MAX"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "特采最大");//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_STRING, tep0002["CODE"].ToString() + "_SMELT_CHEMI_FLAG");		//压入列名(元素代码) 
			bcls_ret->Tables[0].Columns[tep0002["CODE"].ToString() + "_SMELT_CHEMI_FLAG"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "判定及打质保书标记");//压入列标题(元素名)
		}
		cmd_inq.Close();*/
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_CREATOR");
		bcls_ret->Tables[0].Columns["REC_CREATOR"].set_Caption("记录创建责任者");
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "REC_CREATE_TIME");
		bcls_ret->Tables[0].Columns["REC_CREATE_TIME"].set_Caption("记录创建时刻");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_REVISOR");
		bcls_ret->Tables[0].Columns["REC_REVISOR"].set_Caption("记录修改责任者");
		bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "REC_REVISE_TIME");
		bcls_ret->Tables[0].Columns["REC_REVISE_TIME"].set_Caption("记录修改时刻");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING, "VERSION");
		//bcls_ret->Tables[0].Columns["VERSION"].set_Caption("版次");
		//flag=0,画面load元素信息用，不需取值，跳出
		if (q_flag == 0)
		{
			return doFlag;
		}

		Log::Trace("", "", "SELECT TQMTS9A");
		/****************************************************************/
		/*******************从实绩表中读出元素值*************************/
		/****************************************************************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句

			sqlstr = " SELECT * "
				"   FROM (SELECT ROW_NUMBER() OVER() ROW_ID,A.* "  //DB2须用这句话
				"           FROM (SELECT DISTINCT ST_NO,FACTORY_DIV,WHOLE_BACKLOG_CODE,WHOLE_BACKLOG_SEQ "
				"                   FROM TQMTS9A  WHERE base_code = @base_code ";
			if (q_st_no.Trim() != "")	sqlstr = sqlstr + " AND ST_NO LIKE @st_no||'%' ";
			if (q_whole_backlog_code.Trim() != "")	sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			if (q_factory_div.Trim() != "")	sqlstr = sqlstr + " AND FACTORY_DIV = @factory_div ";
			sqlstr = sqlstr + "                  ORDER BY ST_NO ASC, FACTORY_DIV ASC "
				"                ) A "
				"        ) "
				"  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";

			break;
			
		}
		cmd_inq.SetCommandText(sqlstr);
		/*cmd_inq.Parameters.Set("st_no", q_st_no);
		cmd_inq.Parameters.Set("whole_backlog_code", q_whole_backlog_code);*/
		cmd_inq.Parameters.Set("factory_div", q_factory_div);
		cmd_inq.Parameters.Set("i_now_record", i_now_record);
		cmd_inq.Parameters.Set("i_every_page", i_every_page);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			row_id = cmd_inq.GetDecimal(1);
			tqmts9a["ST_NO"] = cmd_inq.GetString(2);
			tqmts9a["FACTORY_DIV"] = cmd_inq.GetString(3);
			tqmts9a["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(4);
			tqmts9a["WHOLE_BACKLOG_SEQ"] = cmd_inq.GetDecimal(5);

			Log::Trace("", "", "tqmts9a.ST_NO = [{0}]", (const char*)tqmts9a["ST_NO"].ToString());
			Log::Trace("", "", "tqmts9a.WHOLE_BACKLOG_CODE = [{0}]", (const char*)tqmts9a["WHOLE_BACKLOG_CODE"].ToString());

			//当页的记录返回前台
			CDataRow& row = bcls_ret->Tables[0].Rows.Add();
			row["ST_NO"] = tqmts9a["ST_NO"];
			row["FACTORY_DIV"] = tqmts9a["FACTORY_DIV"];
			row["WHOLE_BACKLOG_CODE"] = tqmts9a["WHOLE_BACKLOG_CODE"];
			row["WHOLE_BACKLOG_SEQ"] = tqmts9a["WHOLE_BACKLOG_SEQ"];





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
				"  FROM (SELECT DISTINCT ST_NO,FACTORY_DIV,WHOLE_BACKLOG_CODE,WHOLE_BACKLOG_SEQ "
				"          FROM TQMTS9A  WHERE base_code = @base_code ";
			if (q_st_no.Trim() != "")	sqlstr = sqlstr + " AND ST_NO LIKE @st_no||'%' ";
			if (q_whole_backlog_code.Trim() != "")	sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			if (q_factory_div.Trim() != "")	sqlstr = sqlstr + " AND FACTORY_DIV = @factory_div ";
			sqlstr = sqlstr + "       )";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		/*cmd_inq.Parameters.Set("st_no", q_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("whole_backlog_code", q_whole_backlog_code);*/
		cmd_inq.Parameters.Set("steel_group", q_steel_group);
		i_total_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		CDataRow& row = bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[0]["TOTAL_COUNT"] = i_total_count;
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
