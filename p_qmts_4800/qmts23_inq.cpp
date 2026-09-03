/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-05
Description: 炼钢材料处置画面查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 炼钢材料处置画面查询
/// <para>
/// 1.炼钢材料处置画面查询。
/// 
/// </para>
/// <para>数据库表：TMMSM01(物料主档表)				</para>
/// <para>主调用函数：前台QMTS23画面F2查询按钮		</para>
/// <para>需调用函数：								</para>
/// </summary>
/// <param name="HEAT_NO">熔炼号		</param>
/// <param name="MAT_NO">材料号等		</param>
/// <param name="FLAG">查询标记			</param>
/// <returns>  </returns>
===========================================================</remark>*/
//本程序目前无用！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

// service入口
BM2F_ENTERACE(qmts23_inq)


int f_qmts23_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int q_flag = 0;
	CString s_mat_no = "";
	CString s_order_no = "";
	CString s_pono = "";
	CString s_mat_status = "";
	CString s_heat_no = "";
	CString s_sg_sign = "";
	CString s_slab_cut_time_from = "";
	CString s_slab_cut_time_to = "";
	CString s_hold_time_from = "";
	CString s_hold_time_to = "";
	CString s_hold_flag = "";
	CString s_st_empty_flag = "";
	int  i_every_page = 50;
	int  i_now_record = 1;

	CDecimal row_id = 0;
	CDecimal i_total_count = 0;
	
	CString sqlstr = "";

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 获得输入参数 */
		q_flag = bcls_rec->Tables[0].Rows[0]["q_flag"].ToDecimal().ToInt16();
		s_mat_status = bcls_rec->Tables[0].Rows[0]["mat_status"].ToString().Trim();
		s_mat_no = bcls_rec->Tables[0].Rows[0]["mat_no"].ToString().Trim();
		s_order_no = bcls_rec->Tables[0].Rows[0]["order_no"].ToString().Trim();
		s_pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();	
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		s_sg_sign = bcls_rec->Tables[0].Rows[0]["sg_sign"].ToString().Trim();
		s_slab_cut_time_from = bcls_rec->Tables[0].Rows[0]["slab_cut_time_from"].ToString().Trim();
		s_slab_cut_time_to = bcls_rec->Tables[0].Rows[0]["slab_cut_time_to"].ToString().Trim();
		s_hold_time_from = bcls_rec->Tables[0].Rows[0]["hold_time_from"].ToString().Trim();
		s_hold_time_to = bcls_rec->Tables[0].Rows[0]["hold_time_to"].ToString().Trim();
		s_hold_flag = bcls_rec->Tables[0].Rows[0]["hold_flag"].ToString().Trim();
		s_st_empty_flag = bcls_rec->Tables[0].Rows[0]["st_empty_flag"].ToString().Trim();
		i_every_page = bcls_rec->Tables[0].Rows[0]["every_page"].ToDecimal().ToInt32();/*每页显示的记录数*/
		i_now_record = bcls_rec->Tables[0].Rows[0]["now_record"].ToDecimal().ToInt32();/*开始记录序号 i_every_page*PageFrom*/

		Log::Trace("", "","qmts23_inq IN:---q_flag = [{0}]",q_flag);
		Log::Trace("", "","qmts23_inq IN:---s_mat_status = [{0}]",(const char*)s_mat_status);
		Log::Trace("", "","qmts23_inq IN:---s_mat_no = [{0}]",(const char*)s_mat_no);
		Log::Trace("", "", "qmts23_inq IN:---s_order_no = [{0}]", (const char*)s_order_no);
		Log::Trace("", "", "qmts23_inq IN:---s_pono = [{0}]", (const char*)s_pono);
		Log::Trace("", "","qmts23_inq IN:---s_heat_no = [{0}]",(const char*)s_heat_no);
		Log::Trace("", "","qmts23_inq IN:---s_sg_sign = [{0}]",(const char*)s_sg_sign);
		Log::Trace("", "","qmts23_inq IN:---s_hold_flag = [{0}]",(const char*)s_hold_flag);
		Log::Trace("", "","qmts23_inq IN:---s_st_empty_flag = [{0}]",(const char*)s_st_empty_flag);
		Log::Trace("", "","qmts23_inq IN:---s_slab_cut_time_from[{0}], s_slab_cut_time_to[{1}]", (const char*)s_slab_cut_time_from, (const char*)s_slab_cut_time_to);
		Log::Trace("", "","qmts23_inq IN:---s_hold_time_from[{0}], s_hold_time_to[{1}]", (const char*)s_hold_time_from, (const char*)s_hold_time_to);
		Log::Trace("", "","qmts23_inq IN:---i_every_page[{0}], i_now_record[{1}]",i_every_page,i_now_record);

		bcls_ret->Tables[0].Columns.Add(tmmsm01);
		if (q_flag == 1) //查询材料信息
		{
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					sqlstr = " SELECT * "
						"   FROM (SELECT ROW_NUMBER() over() as ROW_ID, A.* "
						"           FROM (SELECT * FROM TMMSM01 "
						"                  WHERE MAT_STATUS LIKE @s_mat_status||'%' "
						"                    AND MAT_NO LIKE '%'||@s_mat_no||'%' "
						"                    AND ORDER_NO LIKE '%'||@s_order_no||'%' "
						"                    AND PONO LIKE '%'||@s_pono||'%' "
						"                    AND HEAT_NO LIKE '%'||@s_heat_no||'%' "
						"                    AND HOLD_FLAG = @s_hold_flag "
						"                    AND SG_SIGN LIKE '%'||@s_sg_sign||'%' ";
					if (s_slab_cut_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(SLAB_CUT_TIME,1,8) >= @s_slab_cut_time_from ";
					if (s_slab_cut_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(SLAB_CUT_TIME,1,8) <= @s_slab_cut_time_to ";
					if (s_hold_time_from.Trim() != "")  sqlstr = sqlstr + " AND (substr(HOLD_TIME,1,8) >= @s_hold_time_from OR substr(MNG_HOLD_TIME,1,8) >= @s_hold_time_from)";
					if (s_hold_time_to.Trim() != "")  sqlstr = sqlstr + " AND (substr(HOLD_TIME,1,8) <= @s_hold_time_to OR substr(MNG_HOLD_TIME,1,8) <= @s_hold_time_to)";
					sqlstr = sqlstr + "                    AND FIN_ST_NO LIKE DECODE(@s_st_empty_flag,'1',' ','%') "
						"                  ORDER BY MAT_NO ASC "
						"                ) A "
						"        ) "
						" WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";
					break;
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM (SELECT ROWNUM ROW_ID,A.* "
							 "           FROM (SELECT * FROM TMMSM01 "
							 "                  WHERE MAT_STATUS LIKE @s_mat_status||'%' "
							 "                    AND MAT_NO LIKE '%'||@s_mat_no||'%' "
							 "                    AND ORDER_NO LIKE '%'||@s_order_no||'%' "
							 "                    AND PONO LIKE '%'||@s_pono||'%' "
							 "                    AND HEAT_NO LIKE '%'||@s_heat_no||'%' "
							 "                    AND HOLD_FLAG = @s_hold_flag "
							 "                    AND SG_SIGN LIKE '%'||@s_sg_sign||'%' ";
				if ( s_slab_cut_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(SLAB_CUT_TIME,1,8) >= @s_slab_cut_time_from ";
				if ( s_slab_cut_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(SLAB_CUT_TIME,1,8) <= @s_slab_cut_time_to ";
				if ( s_hold_time_from.Trim() != "")  sqlstr = sqlstr + " AND (substr(HOLD_TIME,1,8) >= @s_hold_time_from OR substr(MNG_HOLD_TIME,1,8) >= @s_hold_time_from)";
				if ( s_hold_time_to.Trim() != "")  sqlstr = sqlstr + " AND (substr(HOLD_TIME,1,8) <= @s_hold_time_to OR substr(MNG_HOLD_TIME,1,8) <= @s_hold_time_to)";
				sqlstr = sqlstr + "                    AND FIN_ST_NO LIKE DECODE(@s_st_empty_flag,'1',' ','%') "
							 "                  ORDER BY MAT_NO ASC "
							 "                ) A "
							 "        ) "
							 " WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";
					break;
			}
			Log::Trace("", "","sql='[{0}]'",(const char*)sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_mat_status", s_mat_status.Trim());
			cmd_inq.Parameters.Set("s_mat_no", s_mat_no.Trim());
			cmd_inq.Parameters.Set("s_order_no", s_order_no.Trim());
			cmd_inq.Parameters.Set("s_pono", s_pono.Trim());
			cmd_inq.Parameters.Set("s_heat_no", s_heat_no.Trim());
			cmd_inq.Parameters.Set("s_hold_flag", s_hold_flag.Trim());
			cmd_inq.Parameters.Set("s_sg_sign", s_sg_sign.Trim());
			cmd_inq.Parameters.Set("s_slab_cut_time_from", s_slab_cut_time_from.Trim());
			cmd_inq.Parameters.Set("s_slab_cut_time_to", s_slab_cut_time_to.Trim());
			cmd_inq.Parameters.Set("s_hold_time_from", s_hold_time_from.Trim());
			cmd_inq.Parameters.Set("s_hold_time_to", s_hold_time_to.Trim());
			cmd_inq.Parameters.Set("s_st_empty_flag", s_st_empty_flag.Trim());
			cmd_inq.Parameters.Set("i_now_record", i_now_record);
			cmd_inq.Parameters.Set("i_every_page", i_every_page);
			cmd_inq.ExecuteReader();
			while(cmd_inq.Read())
			{
				row_id = cmd_inq.GetDecimal(1);
				cmd_inq.Fetch(tmmsm01,2);
				//当页的记录返回前台
				CDataRow& row = bcls_ret->Tables[0].Rows.Add();
				row.Merge(tmmsm01);
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
					sqlstr = " SELECT COUNT(1) "
							 "   FROM TMMSM01 "
							 "  WHERE MAT_STATUS LIKE @s_mat_status||'%' "
							 "    AND MAT_NO LIKE '%'||@s_mat_no||'%' "
							 "    AND ORDER_NO LIKE '%'||@s_order_no||'%' "
							 "    AND PONO LIKE '%'||@s_pono||'%' "
							 "    AND HEAT_NO LIKE '%'||@s_heat_no||'%' "
							 "    AND HOLD_FLAG = @s_hold_flag "
							 "    AND SG_SIGN LIKE '%'||@s_sg_sign||'%' ";
				if ( s_slab_cut_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(SLAB_CUT_TIME,1,8) >= @s_slab_cut_time_from ";
				if ( s_slab_cut_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(SLAB_CUT_TIME,1,8) <= @s_slab_cut_time_to ";
				if ( s_hold_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(HOLD_TIME,1,8) >= @s_hold_time_from ";
				if ( s_hold_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(HOLD_TIME,1,8) <= @s_hold_time_to ";
				sqlstr = sqlstr + "    AND FIN_ST_NO LIKE DECODE(@s_st_empty_flag,'1',' ','%') ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_mat_status", s_mat_status.Trim());
			cmd_inq.Parameters.Set("s_mat_no", s_mat_no.Trim());
			cmd_inq.Parameters.Set("s_order_no", s_order_no.Trim());
			cmd_inq.Parameters.Set("s_pono", s_pono.Trim());
			cmd_inq.Parameters.Set("s_heat_no", s_heat_no.Trim());
			cmd_inq.Parameters.Set("s_hold_flag", s_hold_flag.Trim());
			cmd_inq.Parameters.Set("s_sg_sign", s_sg_sign.Trim());
			cmd_inq.Parameters.Set("s_slab_cut_time_from", s_slab_cut_time_from.Trim());
			cmd_inq.Parameters.Set("s_slab_cut_time_to", s_slab_cut_time_to.Trim());
			cmd_inq.Parameters.Set("s_hold_time_from", s_hold_time_from.Trim());
			cmd_inq.Parameters.Set("s_hold_time_to", s_hold_time_to.Trim());
			cmd_inq.Parameters.Set("s_st_empty_flag", s_st_empty_flag.Trim());
			i_total_count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			CDataRow& row = bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[0]["TOTAL_COUNT"] = i_total_count;
		}
		else //查询材料异常信息
		{
			Log::Trace("", ""," **************查询材料异常信息*****************");
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM TMMSM01 "
							 "  WHERE MAT_NO = @s_mat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_mat_no", s_mat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm01);
				CDataRow& row = bcls_ret->Tables[0].Rows.Add();
				row.Merge(tmmsm01);
			}
			cmd_inq.Close();
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
