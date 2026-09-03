/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-10-01 14:48:51
Description: 工序成分实际查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmts25_h5_inq)

int f_qmts25_h5_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";

	try
	{
		int record_count_per_page = 0; /* 每页记录数 */
		int current_page_no = 0;       /* 需查询的页号,从0开始计数 */
		int start_row = 0;             /* 将要压入outBlock的起始行 */
		/* 获取传入的表名 */
		/* 每页记录数 */
		record_count_per_page = bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
		/* 需查询的页号 */
		current_page_no = bcls_rec->Tables[1].Rows[0]["PAGE_NUM"];
		CDbCommand cmd(conn);

		/*拼接查询sql语句*/
		CString sql = " WITH qmts25 AS("
			"		SELECT"
			"		*"
			"		FROM"
			"		("
			"		SELECT"
			"		T1.ST_SAMPLE_NO,"
			"		T1.HEAT_NO,"
			"		T1.ELM_ACT,"
			"		T1.ELM_OK,"
			"		T1.ELM_CODE"
			"		FROM"
			"		TQMTS25 T1"
			"		)"
			"		PIVOT("
			"		SUM(ELM_ACT),"
			"		SUM(ELM_OK) AS OK"
			"		FOR elm_code IN('001' AS E001,"
			"		'002' AS E002,"
			"		'003' AS E003,"
			"		'004' AS E004,"
			"		'005' AS E005,"
			"		'006' AS E006,"
			"		'007' AS E007,"
			"		'008' AS E008,"
			"		'009' AS E009,"
			"		'010' AS E010,"
			"		'011' AS E011,"
			"		'012' AS E012,"
			"		'013' AS E013,"
			"		'014' AS E014,"
			"		'015' AS E015,"
			"		'016' AS E016,"
			"		'017' AS E017,"
			"		'018' AS E018,"
			"		'019' AS E019,"
			"		'020' AS E020,"
			"		'021' AS E021,"
			"		'022' AS E022,"
			"		'023' AS E023,"
			"		'024' AS E024,"
			"		'025' AS E025,"
			"		'026' AS E026,"
			"		'027' AS E027,"
			"		'028' AS E028,"
			"		'029' AS E029,"
			"		'030' AS E030,"
			"		'031' AS E031,"
			"		'032' AS E032,"
			"		'033' AS E033,"
			"		'034' AS E034,"
			"		'035' AS E035,"
			"		'036' AS E036,"
			"		'037' AS E037,"
			"		'038' AS E038,"
			"		'039' AS E039,"
			"		'040' AS E040,"
			"		'041' AS E041,"
			"		'042' AS E042,"
			"		'043' AS E043,"
			"		'044' AS E044,"
			"		'045' AS E045,"
			"		'046' AS E046,"
			"		'047' AS E047,"
			"		'048' AS E048,"
			"		'049' AS E049,"
			"		'050' AS E050,"
			"		'051' AS E051,"
			"		'052' AS E052,"
			"		'053' AS E053,"
			"		'054' AS E054,"
			"		'055' AS E055,"
			"		'056' AS E056,"
			"		'057' AS E057,"
			"		'058' AS E058,"
			"		'059' AS E059,"
			"		'060' AS E060,"
			"		'061' AS E061,"
			"		'062' AS E062,"
			"		'063' AS E063,"
			"		'064' AS E064,"
			"		'065' AS E065,"
			"		'066' AS E066,"
			"		'067' AS E067,"
			"		'068' AS E068,"
			"		'069' AS E069,"
			"		'070' AS E070,"
			"		'071' AS E071,"
			"		'072' AS E072,"
			"		'073' AS E073,"
			"		'074' AS E074,"
			"		'075' AS E075,"
			"		'076' AS E076,"
			"		'077' AS E077,"
			"		'078' AS E078,"
			"		'079' AS E079,"
			"		'080' AS E080,"
			"		'081' AS E081,"
			"		'082' AS E082,"
			"		'083' AS E083,"
			"		'084' AS E084,"
			"		'085' AS E085,"
			"		'086' AS E086,"
			"		'087' AS E087,"
			"		'088' AS E088,"
			"		'089' AS E089,"
			"		'090' AS E090,"
			"		'091' AS E091,"
			"		'092' AS E092,"
			"		'093' AS E093,"
			"		'094' AS E094,"
			"		'095' AS E095,"
			"		'096' AS E096,"
			"		'097' AS E097,"
			"		'098' AS E098,"
			"		'099' AS E099,"
			"		'100' AS E100,"
			"		'101' AS E101,"
			"		'102' AS E102,"
			"		'103' AS E103,"
			"		'104' AS E104,"
			"		'105' AS E105,"
			"		'106' AS E106,"
			"		'107' AS E107,"
			"		'108' AS E108,"
			"		'109' AS E109,"
			"		'110' AS E110,"
			"		'111' AS E111,"
			"		'112' AS E112,"
			"		'113' AS E113,"
			"		'114' AS E114,"
			"		'115' AS E115,"
			"		'116' AS E116,"
			"		'117' AS E117,"
			"		'118' AS E118,"
			"		'119' AS E119,"
			"		'120' AS E120,"
			"		'121' AS E121,"
			"		'122' AS E122,"
			"		'123' AS E123,"
			"		'124' AS E124,"
			"		'125' AS E125,"
			"		'126' AS E126,"
			"		'127' AS E127,"
			"		'128' AS E128,"
			"		'129' AS E129,"
			"		'130' AS E130,"
			"		'131' AS E131,"
			"		'132' AS E132,"
			"		'133' AS E133,"
			"		'134' AS E134,"
			"		'135' AS E135,"
			"		'136' AS E136,"
			"		'137' AS E137,"
			"		'138' AS E138,"
			"		'139' AS E139,"
			"		'140' AS E140,"
			"		'141' AS E141,"
			"		'142' AS E142,"
			"		'143' AS E143,"
			"		'144' AS E144"
			"		)"
			"		)"
			"		)"
			"		SELECT"
			"		TQMTS23.FIN_ST_NO,"
			"		TQMTS23.ST_NO,"
			"		TQMTS24.ANALYSE_TIME,"
			"		TQMTS24.PONO,"
			"		TQMTS24.HEAT_NO,"
			"		TQMTS24.REP_ELM_SEL_FLAG,"
			"		TQMTS24.SM_PLAN_NO,"
			"		TQMTS24.ST_NO,"
			"		TQMTS24.JUDGE_CODE,"
			"		TQMTS24.ST_SAMPLE_DIV,"
			"		TQMTS24.GAS_TYPE_DIV,"
			"		TQMTS24.ST_SAMPLE_SEQ,"
			"		TQMTS24.PREC_ST_NO,"
			"		TQMTS24.COMPANY_CODE,"
			"		TQMTS24.COMPANY_NAME,"
			"		qmts25.*"
			"		FROM"
			"		TQMTS24"
			"		LEFT JOIN qmts25"
			"		ON"
			"		TQMTS24.ST_SAMPLE_NO = qmts25.ST_SAMPLE_NO"
			"		AND TQMTS24.HEAT_NO = qmts25.HEAT_NO"
			"		LEFT JOIN TQMTS23"
			"		ON"
			"		TQMTS24.HEAT_NO = TQMTS23.HEAT_NO ";

		CString sql_where = " WHERE TQMTS24.ST_SAMPLE_DIV = '1'";
		CString sql_order_by = " ORDER BY TQMTS24.ANALYSE_TIME DESC";

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";


		/*获取查询条件传入列数*/
		//int count_row = bcls_rec->Tables[0].Columns.get_Count();

		//for (int i = 0; i < count_row; i++)
		//{
		//	Log::Trace("", "", "bcls_rec->Tables[0].Rows[0][i].ToString().Trim()= {0}", bcls_rec->Tables[0].Rows[0][i].ToString().Trim());
		//	/*如果查询条件的值为空则跳出*/
		//	if (bcls_rec->Tables[0].Rows[0][i].ToString().Trim().IsEmpty())
		//	{
		//		continue;
		//	}
		//	Log::Trace("", "", "条件查询sql {0}++[{1}],", i, bcls_rec->Tables[0].Columns[i].get_ColumnName() + ":" + bcls_rec->Tables[1].Rows[0][0].ToString().Trim());

		//	sql_where += " AND TQMTS24." + bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString() + " LIKE @" + bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString() + "||'%'";
		//	cmd.Parameters.Set(bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString(), bcls_rec->Tables[0].Rows[i]["OP_VALUE"].ToString().Trim());
		//}

		if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() != "")
		{
			sql_where += " AND TQMTS24.HEAT_NO  LIKE '" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "%'";
			Log::Trace("", "", "HEAT_NO={0}", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim() != "")
		{
			sql_where += " AND TQMTS24.ST_SAMPLE_NO LIKE '" + bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString() + "%'";
			Log::Trace("", "", "ST_SAMPLE_NO={0}",  bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim());
		}




		/*连接sql语句*/
		sqlstr = sql_count + sql + sql_where + ")";
		cmd.SetCommandText(sqlstr);

		/*获取条数*/
		CDecimal rc = cmd.ExecuteScalar();

		/*把值压入RC中，传出前台*/
		bcls_ret->ExtendedProperties.Add("RC", rc.ToString());

		/*完成拼接查询sql*/
		sqlstr = sql + sql_where + sql_order_by;
		Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
		cmd.SetCommandText(sqlstr);

		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > rc.ToDouble())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "current_page_no		= [{0}]", current_page_no);
		Log::Trace("", __FUNCTION__, "record_count_per_page = [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "start_row			    = [{0}]", start_row);

		int count = cmd.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		bcls_ret->Tables[0].set_TableName("TQMTS24");

		// 返回分页总数量信息

		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = rc;
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
