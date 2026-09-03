/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   chenwenqiong
Version:    1.0
Date:     2011-08-12 17:13:56
Description: 钢种对换的成分实绩修改，重新判定该工序的化学成分
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
/// <para>数据库表：TQMTS25(成分实绩表)
</para>
/// <para>主调用函数： f_qmtjp_01(推算板坯最终出钢记号)
///                     f_qmts_jud_01(推算板坯最终出钢记号)
/// <para>需调用函数：  f_qmts_cf00
/// </summary>
/// <param name="ST_NO">炉次最终出钢记号             </param>
///<param name="PONO">制造命令号           </param>
///<param name="i_whole_backlog_code">变更工序           </param>

/// <returns>  </returns>
===========================================================</remark>*/


/*  外部函数申明  */


BM2_FUNCTION_IMPORT
int f_qmts_elm_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//成分判定


BM2_FUNCTION_EXPORT
int f_qmts_stno_chgd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	//APP_BEGIN()
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//变量声明
	int i = 0;
	int  doFlag = 0;
	int  ret = 0;
	CString sqlstr("");
	CString judge_pono = " ";
	int o_flag = 0;
	int upd_count = 0;
	int count24 = 0;
	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tqmts24("TQMTS24");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_upd(conn);
	CDbCommand cmd_upd1(conn);
	CDbCommand cmd_upd2(conn);
	CDbCommand cmd_upd3(conn);
	CDbCommand cmd_upd4(conn);
	CDbCommand cmd_upd5(conn);
	CDbCommand cmd_upd6(conn);
	CDbCommand cmd_upd7(conn);
	CDbCommand cmd_upd8(conn);
	CDbCommand cmd_upd9(conn);
	CDbCommand cmd_upd10(conn);
	CDbCommand cmd_inq(conn);

	try
	{
		//获得输入参数
		CString v_pono = bcls_rec->Tables[0].Rows[0]["PONO_OUT"].ToString().TrimOrBlank();
		CString v_pono_new = bcls_rec->Tables[0].Rows[0]["PONO_IN"].ToString().TrimOrBlank();
		CString v_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE_OUT"].ToString().TrimOrBlank();
		CString v_whole_backlog_code_new = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE_IN"].ToString().TrimOrBlank();
		CString v_st_no_new = bcls_rec->Tables[0].Rows[0]["ST_NO_IN"].ToString().TrimOrBlank();
		CString v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO_OUT"].ToString().TrimOrBlank();
		CString v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();

		//*********************************   1. 获取/检查输入参数   *********************************//
		Log::Trace("", __FUNCTION__, "v_pono[{0}]", v_pono);
		Log::Trace("", __FUNCTION__, "v_st_no[{0}]", v_st_no);
		Log::Trace("", __FUNCTION__, "v_whole_backlog_code[{0}]", v_whole_backlog_code);
		Log::Trace("", __FUNCTION__, "v_pono_new[{0}]", v_pono_new);
		Log::Trace("", __FUNCTION__, "v_st_no_new[{0}]", v_st_no_new);
		Log::Trace("", __FUNCTION__, "v_whole_backlog_code_new[{0}]", v_whole_backlog_code_new);
		Log::Trace("", __FUNCTION__, "v_factory_div[{0}]", v_factory_div);

		if (strcmp(v_pono, " ") == 0)
		{
			sprintf(s.msg, _RES("GCRSS0000028")/*PONO号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (strcmp(v_pono_new, " ") == 0)
		{
			sprintf(s.msg, _RES("GCRSS0000028")/*PONO号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (strcmp(v_st_no, " ") == 0)
		{
			sprintf(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (strcmp(v_whole_backlog_code, " ") == 0)
		{
			sprintf(s.msg, _RES("QM00S0004177")/*请选择或输入工序代码。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (strcmp(v_factory_div, " ") == 0)
		{
			sprintf(s.msg, _RES("QM00S0004396")/*主工序代码不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//校验传入的pono号是否选了代表样，update by yiling  20200323
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default: // 所有数据库适用，通用SQL语句
		//	sqlstr = CString(" select count(*) from tqmts24 t where (t.pono = @v_pono or t.pono = @v_pono_new) and t.rep_elm_sel_flag = '1' ");
		//	break;
		//}

		//cmd_inq.Parameters.Clear();
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("v_pono", v_pono);
		//cmd_inq.Parameters.Set("v_pono_new", v_pono_new);
		//count24 = cmd_inq.ExecuteScalar().ToInt32();
		//cmd_inq.Close();
		//if (count24 > 0)
		//{
		//	sprintf(s.msg, "需变更的制造命令号已选代表样，如要继续变更请至[QMTS25S]画面取消代表样选择后再操作。");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		Log::Trace("", __FUNCTION__, "替换制造命令号开始");
		//***********************替换制造命令号***********************/

		//1.所有PONO都更新成过渡PONO=XXXXX
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS25 "
				" SET pono   = 'XXXXX'  , "
				"  REC_CREATE_TIME = @datetime"
				" WHERE pono = @pono "
				;
			break;
		}
		cmd_upd1.SetCommandText(sqlstr);
		cmd_upd1.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd1.Parameters.Set("datetime", s.datetime);
		upd_count = cmd_upd1.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "1.所有PONO[{0}]都更新成过渡PONO=XXXXX,完成，影响[{1}]行", v_pono, upd_count);

		//2.所有PONO_NEW都更新成PONO
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS25 "
				" SET pono   = @pono  , "
				"  REC_CREATE_TIME = @datetime"
				" WHERE pono = @pono_new "
				;
			break;
		}
		cmd_upd2.SetCommandText(sqlstr);
		cmd_upd2.Parameters.Set("pono_new", v_pono_new);
		cmd_upd2.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd2.Parameters.Set("datetime", s.datetime);
		upd_count = cmd_upd2.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "2.所有PONO_NEW[{0}]都更新成PONO[{1}],完成，影响[{2}]行", v_pono, v_pono_new, upd_count);

		//3.所有过渡PONO(XXXXX)都更新成PONO_NEW
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS25 "
				" SET pono   = @pono_new , "
				"  REC_CREATE_TIME = @datetime"
				" WHERE pono = 'XXXXX' "
				;
			break;
		}
		cmd_upd3.SetCommandText(sqlstr);
		cmd_upd3.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd3.Parameters.Set("datetime", s.datetime);
		upd_count = cmd_upd3.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "3.所有过渡PONO(XXXXX)都更新成PONO_NEW[{0}],完成，影响[{1}]行", v_pono_new, upd_count);

		//***********************当前工序的钢种及判定结果更新v_st_no_new***********************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS25 "
				" SET st_no  = @st_no_new, "
				" elm_ok = 0 "
				" WHERE pono = @pono_new ";
			break;
		}
		cmd_upd4.SetCommandText(sqlstr);
		cmd_upd4.Parameters.Set("st_no_new", v_st_no_new);
		cmd_upd4.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd4.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "当前工序的钢种及判定结果更新v_st_no_new,完成");

		//***********************当前工序的钢种及判定结果更新v_st_no***********************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS25 "
				" SET st_no  = @st_no, "
				" elm_ok = 0 "
				" WHERE pono = @pono ";
			break;
		}
		cmd_upd5.SetCommandText(sqlstr);
		cmd_upd5.Parameters.Set("st_no", v_st_no);
		cmd_upd5.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd5.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "当前工序的钢种及判定结果更新v_st_no,完成");
		Log::Trace("", __FUNCTION__, "炉次主表更新 pono 成v_pono_new开始");
		//***********************炉次主表更新 pono 成v_pono_new***********************/

		//1.所有PONO都更新成过渡PONO=XXXXX
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS24 "
				" SET pono   = 'XXXXX' "
				" WHERE  pono = @pono ";
			break;
		}
		cmd_upd6.SetCommandText(sqlstr);
		cmd_upd6.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd6.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "1.所有PONO都更新成过渡PONO=XXXXX");

		//2.所有PONO_NEW都更新成PONO
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS24 "
				" SET pono   = @pono "
				" WHERE   pono = @pono_new ";
			break;
		}
		cmd_upd7.SetCommandText(sqlstr);
		cmd_upd7.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd7.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd7.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "2.所有PONO_NEW都更新成PONO");

		//3.所有过渡PONO(XXXXX)都更新成PONO_NEW
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS24 "
				" SET pono   = @pono_new "
				" WHERE   pono = 'XXXXX' ";
			break;
		}
		cmd_upd8.SetCommandText(sqlstr);
		cmd_upd8.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd8.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "3.所有过渡PONO(XXXXX)都更新成PONO_NEW");

		//更新ST_NO成v_st_no_new
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS24 "
				" SET st_no  = @st_no_new, "
				" judge_code = ' ' "
				" WHERE   pono = @pono_new "
				;
			break;
		}
		cmd_upd9.SetCommandText(sqlstr);
		cmd_upd9.Parameters.Set("st_no_new", v_st_no_new);
		cmd_upd9.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd9.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "更新ST_NO成v_st_no_new，完成");

		//更新ST_NO成v_st_no
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS24 "
				" SET st_no  = @st_no, "
				" judge_code = ' ' "
				" WHERE  pono = @pono "
				;
			break;
		}
		cmd_upd10.SetCommandText(sqlstr);
		cmd_upd10.Parameters.Set("st_no", v_st_no);
		cmd_upd10.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd10.ExecuteNonQuery();
		Log::Trace("", __FUNCTION__, "更新ST_NO成v_st_no，完成");
		/////update by yiling 20190805  应西王冯晓轶要求，炉次交换的时候，漏更新了TQMTS23表，接收成分时，取的是TQMTS23的PONO，在炉次更新后上的成分PONO会跟物料对不上
		Log::Trace("", __FUNCTION__, "炉次主表TQMTS23更新 pono 成v_pono_new开始");
		//***********************炉次主表更新 pono 成v_pono_new***********************/

		//1.所有PONO都更新成过渡PONO=XXXXX
		sqlstr = " UPDATE TQMTS23 "
			" SET pono   = 'XXXXX' "
			" WHERE  pono = @pono ";
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();
		Log::Trace("", __FUNCTION__, "1.所有PONO都更新成过渡PONO=XXXXX");

		//2.所有PONO_NEW都更新成PONO
		sqlstr = " UPDATE TQMTS23 "
			" SET pono   = @pono "
			" WHERE   pono = @pono_new ";
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();
		Log::Trace("", __FUNCTION__, "2.所有PONO_NEW都更新成PONO");

		//3.所有过渡PONO(XXXXX)都更新成PONO_NEW
		sqlstr = " UPDATE TQMTS23 "
			" SET pono   = @pono_new "
			" WHERE   pono = 'XXXXX' ";
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();
		Log::Trace("", __FUNCTION__, "3.所有过渡PONO(XXXXX)都更新成PONO_NEW");

		//更新ST_NO成v_st_no_new
		sqlstr = " UPDATE TQMTS23 "
			" SET st_no  = @st_no_new "
			" WHERE   pono = @pono_new "
			;
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("st_no_new", v_st_no_new);
		cmd_upd.Parameters.Set("pono_new", v_pono_new.TrimOrBlank());
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();
		Log::Trace("", __FUNCTION__, "更新ST_NO成v_st_no_new，完成");

		//更新ST_NO成v_st_no
		sqlstr = " UPDATE TQMTS23 "
			" SET st_no  = @st_no "
			" WHERE  pono = @pono "
			;
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("st_no", v_st_no);
		cmd_upd.Parameters.Set("pono", v_pono.TrimOrBlank());
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();
		Log::Trace("", __FUNCTION__, "更新ST_NO成v_st_no，完成");

		//***********************重新判定PONO_NEW的成分是否合格***********************/
		for (i = 0; i <= 1; i++)
		{
			if (i == 0) judge_pono = v_pono_new;
			if (i == 1) judge_pono = v_pono;
			if (i == 2) break;

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					" FROM TQMTS24 "
					" WHERE pono = @judge_pono "
					" ORDER BY st_sample_no ASC ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("judge_pono", judge_pono);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts24);
				Log::Trace("", __FUNCTION__, "重新判定PONO的成分是否合格，进入，tqmts24.HEAT_NO=[{0}]，tqmts24.PONO =[{1}],ST_SAMPLE_NO=[{2}]", tqmts24["HEAT_NO"].ToString(), tqmts24["PONO"].ToString(), tqmts24["ST_SAMPLE_NO"].ToString());
				//==================================================================================================
				bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
				bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts24["HEAT_NO"];
				bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
				bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts24["ST_NO"];
				bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts24["PONO"];

				Log::Trace("", __FUNCTION__, "tqmts24.SAMPLE_IFGOOD =[{0}]，tqmts24.ST_SAMPLE_DIV=[{1}]", tqmts24["SAMPLE_IFGOOD"].ToString(), tqmts24["ST_SAMPLE_DIV"].ToString());
				//试样良的话参与成分判定

				Log::Trace("", __FUNCTION__, "进入调用f_qmts_jud");
				doFlag = f_qmts_elm_jud(&bcls_rec_f, &bcls_ret_f, conn);
				if (doFlag != 0)
				{
					Log::Trace("", __FUNCTION__, "f_qmts_jud() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}
			cmd_inq.Close();
		}

		o_flag = 0;

		bcls_ret->Tables[0].Columns.Clear();
		bcls_ret->Tables[0].Columns.Add(DT_INT16, "CHK_FLAG");
		CDataRow& row_ret = bcls_ret->Tables[0].Rows.Add();
		row_ret["CHK_FLAG"] = o_flag;
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
	//在函数退出前，统一Close()操作
	cmd_upd1.Close();
	cmd_upd2.Close();
	cmd_upd3.Close();
	cmd_upd4.Close();
	cmd_upd5.Close();
	cmd_upd6.Close();
	cmd_upd7.Close();
	cmd_upd8.Close();
	cmd_upd9.Close();
	cmd_upd10.Close();
	return doFlag;
}

