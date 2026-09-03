/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2014-08-25
Description: 成分判定函数   /////韶钢增加nb_v_ti；cr_mo；cr_ni_cu_mo三类混杂元素 ,update by yiling 20150727
**************************************************/


/*
				   _ooOoo_
				   o8888888o
				   88" . "88
				   (| -_- |)
				   O\  =  /O
				   ____/`---'\____
				   .'  \\|     |//  `.
				   /  \\|||  :  |||//  \
				   /  _||||| -:- |||||-  \
				   |   | \\\  -  /// |   |
				   | \_|  ''\---/''  |   |
				   \  .-\__  `-`  ___/-. /
				   ___`. .'  /--.--\  `. . __
				   ."" '<  `.___\_<|>_/___.'  >'"".
				   | | :  `- \`.;`\ _ /`;.`/ - ` : | |
				   \  \ `-.   \_ __\ /__ _/   .-` /  /
				   ======`-.____`-.___\_____/___.-`____.-'======
				   `=---='
				   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				   佛祖保佑       永无BUG
				   */



//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中




#include "math.h"


/*  外部函数申明  */

BM2_FUNCTION_IMPORT
int f_qmts_spe_sm(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//计算组合元素


BM2_FUNCTION_EXPORT
int f_qmts_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int			doFlag = 0;
	int			i = 0;
	int			fetchRowCount = 0;

	bool		isDec;

	CString		s_table_name = " ";	//表名
	CString		s_heat_no = " ";
	CString		s_st_sample_no = " ";
	CString     s_pono = " ";
	CString     s_factory_div = "";
	CString		s_whole_backlog_code = "";
	CString		s_st_sample_div = "";
	CString		s_gas_type_div = "";

	CString		s_o_gas_div = "";
	CString		s_n_gas_div = "";
	CString		s_h_gas_div = "";

	CDecimal	cu_ni_cr = 0;
	CDecimal	as_sn_nb_v_ti_zr_b = 0;
	CDecimal	nb_v_ti = 0;
	CDecimal	cr_mo = 0;
	CDecimal	cr_ni_cu_mo = 0;
	CDecimal	elm_act = 0;

	int			elm_ok = 0;	//成分合否标志 0:未判定,1:主试不合且特采不合,2:大于主试最大且无特采,4:小于主试最小且无特采,3:主试不合但特采合,8:主试合,9:不需判
	int			elm_0_num = 0;	//未收到实绩的元素数量
	int			elm_1_num = 0;	//判定结果为不合的元素数量

	CString		s_judge_code = "";	//最终判定结果 0:未判定,1:合格,2:不合格

	CString		sqlstr = "";


	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tqmts09("TQMTS09");
	CModel tqmts25("TQMTS25");
	CModel tqmts2c("TQMTS2C");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_upd(conn);

	Log::Trace("", "", "进入函数f_qmts_jud");

	EIClass bcls_elm;//存放元素标准
	bcls_elm.Tables[0].Columns.Add(tqmts02);
	bcls_elm.Tables[0].Columns.Add(DT_INT16, "ELM_OK");


	try
	{
		Log::Trace("", "", "1进入函数f_qmts_jud");

		/*获得传入参数*/
		s_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			s_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ST_SAMPLE_NO"))
		{
			s_st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().TrimOrBlank();
		}
		tqmts02["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
		{
			s_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().TrimOrBlank();
		}
		//s_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().TrimOrBlank();

		Log::Trace("", __FUNCTION__, "f_qmts_jud IN:---s_table_name		[{0}]", s_table_name);
		Log::Trace("", __FUNCTION__, "f_qmts_jud IN:---s_heat_no			[{0}]", s_heat_no);
		Log::Trace("", __FUNCTION__, "f_qmts_jud IN:---s_st_sample_no		[{0}]", s_st_sample_no);
		Log::Trace("", __FUNCTION__, "f_qmts_jud IN:---tqmts02.ST_NO		[{0}]", tqmts02["ST_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"f_qmts_jud IN:---s_pono				[{0}]", s_pono);

		//校验传入参数
		if (" " == s_table_name)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数table_name不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (" " == s_heat_no)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数heat_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (" " == s_st_sample_no && (s_table_name == "TQMTS25" || s_table_name == "TQMTS2C"))
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数st_sample_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (" " == tqmts02["ST_NO"].ToString())
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数st_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/*if (" " == s_pono)
		{
		Log::Trace("", __FUNCTION__, "ERROR------[传入参数pono不允许为空]");
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//获取气体试样区分
		//switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:						// 所有数据库适用，通用SQL语句
		//		sqlstr = " SELECT NVL(TRIM(O_GAS_DIV),'0'),NVL(TRIM(N_GAS_DIV),'0'),NVL(TRIM(H_GAS_DIV),'0') "
		//				 "   FROM TQMTS0X "
		//				 "  WHERE ST_NO = @st_no ";
		//		break;
		//}
		//cmd_inq.SetCommandText(sqlstr);		
		//cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	s_o_gas_div = cmd_inq.GetString(1);
		//	s_n_gas_div = cmd_inq.GetString(2);
		//	s_h_gas_div = cmd_inq.GetString(3);
		//}
		//cmd_inq.Close();
		tqmts02["ELM_CODE"] = "016";  //O  update by yiling 20180906
		if (tqmts02.QueryCount("ST_NO,ELM_CODE") > 0)
		{
			s_o_gas_div = "1";
		}
		tqmts02["ELM_CODE"] = "014";  //N
		if (tqmts02.QueryCount("ST_NO,ELM_CODE") > 0)
		{
			s_n_gas_div = "1";
		}
		tqmts02["ELM_CODE"] = "001";  //H
		if (tqmts02.QueryCount("ST_NO,ELM_CODE") > 0)
		{
			s_h_gas_div = "1";
		}
		Log::Trace("", __FUNCTION__, "气体试样区分 O[{0}]N[{1}]H[{2}]", s_o_gas_div, s_n_gas_div, s_h_gas_div);


		/*------------------------  查询混杂元素标准信息  ------------------------*/
		//从TQMTS04中查询指定的混杂元素判定标准代码，再从TQMTS09中取出标准
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT a.* "
				"   FROM TQMTS09 a,TQMTS0X b "
				"  WHERE a.HZGL = b.HZGL "
				"    and b.ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts09);
		}
		else
		{
			tqmts09["HZGL"] = " ";
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "tqmts09.HZGL = [{0}]", tqmts09["HZGL"].ToString());


		if ("TQMTS25" == s_table_name)
		{
			/*------------------------  查询试样成分主信息  ------------------------*/
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV "
					"   FROM TQMTS24 "
					"  WHERE HEAT_NO = @heat_no "
					"    AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", s_heat_no);
			cmd_inq.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				s_whole_backlog_code = cmd_inq.GetString(1);
				s_st_sample_div = cmd_inq.GetString(2);
				s_gas_type_div = cmd_inq.GetString(3);
			}
			cmd_inq.Close();
			Log::Trace("", __FUNCTION__, "s_whole_backlog_code	= [{0}]", s_whole_backlog_code);
			Log::Trace("", __FUNCTION__, "s_st_sample_div		= [{0}]", s_st_sample_div);
			Log::Trace("", __FUNCTION__, "s_gas_type_div		= [{0}]", s_gas_type_div);

			//脱磷采用转炉成分标准 add by FXY 2012-10-15
			/*if ("P" == s_whole_backlog_code.TrimOrBlank())
			{
			s_whole_backlog_code = "B";
			}*/
		}

		////////查询厂别，update by yiling 20170106
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT ST_NO,FACTORY_DIV FROM"
				" (SELECT ST_NO,FACTORY_DIV FROM TPSSM11 WHERE HEAT_NO = @heat_no "
				" UNION "
				"  SELECT ST_NO,FACTORY_DIV FROM TPSSM41 WHERE HEAT_NO = @heat_no )"
				;
			break;
		}
		Log::Trace("", "", "sqlstr = [{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			//s_factory_div = cmd_inq.GetString(2).Trim().Substring(1, 1);
			s_factory_div = cmd_inq.GetString(2).Trim();
			Log::Trace("", __FUNCTION__, "s_factory_div		= [{0}]", s_factory_div);

			if (s_factory_div == "1" || s_factory_div == "A" || s_factory_div == "A10")
			{
				s_factory_div = "A";
			}
			else
			{
				s_factory_div = "B";
			}
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "s_factory_div		= [{0}]", s_factory_div);

		/*------------------------  查询工序成分标准信息  ------------------------*/
		fetchRowCount = 0;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			sqlstr = " SELECT * "
				"   FROM TQMTS02 "
				"  WHERE ST_NO = @st_no "
				"    AND SMELT_CHEMI_FLAG >= '3'"
				"  AND FACTORY_DIV =@s_factory_div"
				//" AND  SUBSTR(ELM_CODE,1,1) NOT BETWEEN 'A' AND 'Z' "
				;//不包括组合元素，因组合元素后面调的spe_sm函数中会单独算和判
			if ("TQMTS25" == s_table_name)
				sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			else
				sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = (SELECT IC_CC_FLAG FROM TQMTS0X WHERE ST_NO = @st_no AND FACTORY_DIV =@s_factory_div ) ";
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS02 "
				"  WHERE ST_NO = @st_no "
				"   AND SMELT_CHEMI_FLAG >= '3'"
				"  AND FACTORY_DIV =@s_factory_div"
				//" AND  SUBSTR(ELM_CODE,1,1) NOT BETWEEN 'A' AND 'Z' "
				;
			if ("TQMTS25" == s_table_name)
				sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
			else
				sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = (SELECT IC_CC_FLAG FROM TQMTS0X WHERE ST_NO = @st_no AND FACTORY_DIV =@s_factory_div ) ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
		cmd_inq.Parameters.Set("whole_backlog_code", s_whole_backlog_code);
		cmd_inq.Parameters.Set("s_factory_div", s_factory_div);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;
			cmd_inq.Fetch(tqmts02);
			Log::Trace("", __FUNCTION__, " cmd_inq.Fetch(tqmts02) [{1}]:[{2}]", i, tqmts02["ELM_CODE"].ToString(), tqmts02["ELM_NAME"].ToString());
			CDataRow& row = bcls_elm.Tables[0].Rows.Add();
			row.Merge(tqmts02);
			row["ELM_OK"] = elm_ok;
		}
		cmd_inq.Close();
		Log::Trace("", "", " tqmts02.ST_NO[{0}]", tqmts02["ST_NO"].ToString());
		Log::Trace("", "", "s_whole_backlog_code[{0}]", s_whole_backlog_code);
		Log::Trace("", "", "s_factory_div[{0}]", s_factory_div);
		Log::Trace("", "", "sqlstr[{0}]", sqlstr);
		Log::Trace("", "", "f_qmts_judfetchRowCount = [{0}]", fetchRowCount);
		if (fetchRowCount == 0)  ////////用厂别没查到，用‘ ’查，update by yiling 20170106
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				sqlstr = " SELECT * "
					"   FROM TQMTS02 "
					"  WHERE ST_NO = @st_no "
					"  AND FACTORY_DIV =' '"
					"    AND SMELT_CHEMI_FLAG >= '3'"
					//" AND  SUBSTR(ELM_CODE,1,1) NOT BETWEEN 'A' AND 'Z' "
					;
				if ("TQMTS25" == s_table_name)
					sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
				else
					sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = (SELECT IC_CC_FLAG FROM TQMTS0X WHERE ST_NO = @st_no AND FACTORY_DIV =@s_factory_div ) ";
				break;
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TQMTS02 "
					"  WHERE ST_NO = @st_no "
					"   AND SMELT_CHEMI_FLAG >= '3'"
					"  AND FACTORY_DIV =' '"
					//" AND  SUBSTR(ELM_CODE,1,1) NOT BETWEEN 'A' AND 'Z' "
					;
				if ("TQMTS25" == s_table_name)
					sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
				else
					sqlstr = sqlstr + " AND WHOLE_BACKLOG_CODE = (SELECT IC_CC_FLAG FROM TQMTS0X WHERE ST_NO = @st_no AND FACTORY_DIV =@s_factory_div ) ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
			cmd_inq.Parameters.Set("whole_backlog_code", s_whole_backlog_code);
			cmd_inq.Parameters.Set("s_factory_div", s_factory_div);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				fetchRowCount++;
				cmd_inq.Fetch(tqmts02);
				Log::Trace("", __FUNCTION__, " cmd_inq.Fetch(tqmts02) [{1}]:[{2}]", i, tqmts02["ELM_CODE"].ToString(), tqmts02["ELM_NAME"].ToString());
				CDataRow& row = bcls_elm.Tables[0].Rows.Add();
				row.Merge(tqmts02);
				row["ELM_OK"] = elm_ok;
			}
			cmd_inq.Close();
			Log::Trace("", "", "22222f_qmts_judfetchRowCount = [{0}]", fetchRowCount);
		}
		Log::Trace("", "", "111s_whole_backlog_code = [{0}]", s_whole_backlog_code);

		if (fetchRowCount <= 0 && (s_whole_backlog_code == "C" || s_whole_backlog_code == "I" ))  ////连铸或模铸上必须维护要判定的元素
		{
			sprintf(s.msg, "出钢记号[%s]在工序[%s]中的成分标准不存在，请先维护成分标准", (const char*)tqmts02["ST_NO"].ToString(), (const char*)s_whole_backlog_code);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		else if (fetchRowCount <= 0 && s_whole_backlog_code == "") {
			sprintf(s.msg, "出钢记号[%s]的成分标准不存在，请先维护成分标准", (const char*)tqmts02["ST_NO"].ToString(), (const char*)s_whole_backlog_code);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if ("1" == s_st_sample_div)//QV样才计算组合元素
		{
			//计算组合元素
			doFlag = f_qmts_spe_sm(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				Log::Trace("", "", "f_qmts_spe() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		for (i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			CDataRow& row = bcls_elm.Tables[0].Rows.Add();
			row["ELM_CODE"] = bcls_ret->Tables[0].Rows[i]["ELM_CODE"];
			row["MAIN_MIN"] = bcls_ret->Tables[0].Rows[i]["MAIN_MIN"];
			row["MAIN_MAX"] = bcls_ret->Tables[0].Rows[i]["MAIN_MAX"];
			row["ELM_OK"] = elm_ok;
		}


		/*------------------------  元素判定  ------------------------*/
		//对元素逐次判定
		for (i = 0; i < bcls_elm.Tables[0].Rows.get_Count(); i++)
		{
			//1.取元素标准值
			tqmts02.MergeFrom(bcls_elm.Tables[0].Rows[i]);

			Log::Trace("", __FUNCTION__, "========== 第 [{0}] 项 ------ 元素代码-名称 [{1}]:[{2}]==============================", i, tqmts02["ELM_CODE"].ToString(), tqmts02["ELM_NAME"].ToString());
			Log::Trace("", __FUNCTION__, " 主试[{0}/{1}]-特采[{2}/{3}]-目标[{4}] ----", tqmts02["MAIN_MIN"].ToDecimal(), tqmts02["MAIN_MAX"].ToDecimal(), tqmts02["SPE_MIN"].ToDecimal(), tqmts02["SPE_MAX"].ToDecimal(), tqmts02["MAIN_AIM"].ToDecimal());

			//初始化判定结果
			elm_ok = 0;


			//2.取成分元素实绩值
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT ";
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " ELM_ACT_OLD ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " ELM_VALUE ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " ELM_ACT_OLD ";
				sqlstr = sqlstr + "   FROM " + s_table_name + "  WHERE ELM_CODE = @elm_code ";
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
			cmd_inq.Parameters.Set("heat_no", s_heat_no);
			cmd_inq.Parameters.Set("st_sample_no", s_st_sample_no);

			Log::Trace("", __FUNCTION__, " select sqlstr[{0}] ", sqlstr);
			Log::Trace("", __FUNCTION__, " select elm_code[{0}] ", tqmts02["ELM_CODE"].ToString());
			Log::Trace("", __FUNCTION__, " select st_sample_no[{0}] ", s_st_sample_no);

			cmd_inq.ExecuteReader();
			if (cmd_inq.Read()) //有实绩
			{
				elm_act = cmd_inq.GetDecimal(1);

				Log::Trace("", __FUNCTION__, " 有实绩：实绩值 [{0}] ", elm_act);
				/////////////begin修约update by yiling 20170803
				if (tqmts02["ELM_ACCU"].ToDecimal() > 0 && (s_whole_backlog_code == "C" || s_whole_backlog_code == "L"))
				{
					CDecimal v_elm_format = pow(10.0, tqmts02["ELM_ACCU"].ToDecimal().ToDouble());
					elm_act = elm_act * v_elm_format;
					int z_value = floor(elm_act.ToDouble());//取传入长度的整数部分
					Log::Trace("", __FUNCTION__, " z_value [{0}] elm_act[{1}] ", z_value, elm_act);

					float x_value = (elm_act.ToDouble() - z_value);//取传入长度的小数部分
					Log::Trace("", __FUNCTION__, " x_value [{0}]  ", x_value);

					if (x_value == 0.5)
					{
						if (z_value % 2 == 0)
						{
							elm_act = z_value;
						}
						else
						{
							elm_act = z_value + 1;
						}
					}
					else
					{
						elm_act = floor(elm_act.ToDouble() + 0.5);
					}
					elm_act = elm_act.ToDouble() / v_elm_format.ToDouble();
					Log::Trace("", __FUNCTION__, " 修约后elm_act [{0}]  ", elm_act);
				}
				if ("TQMTS25" == s_table_name)
				{
					tqmts25["ELM_CODE"] = tqmts02["ELM_CODE"];
					tqmts25["ST_SAMPLE_NO"] = s_st_sample_no;
					tqmts25["HEAT_NO"] = s_heat_no;
					tqmts25["REC_REVISOR"] = s.userid;
					tqmts25["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts25["ELM_ACT"] = elm_act;
					tqmts25.Update("ELM_ACT,REC_REVISOR,REC_REVISE_TIME", "HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
				}
				if ("TQMTS2C" == s_table_name)
				{
					tqmts2c["ELM_CODE"] = tqmts02["ELM_CODE"];
					tqmts2c["ST_SAMPLE_NO"] = s_st_sample_no;
					tqmts2c["REC_REVISOR"] = s.userid;
					tqmts2c["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts2c["ELM_VALUE"] = elm_act;
					Log::Trace("", __FUNCTION__, "TQMTS2C： elm_act[{0}] ", elm_act);
					tqmts2c.Update("ELM_VALUE,REC_REVISOR,REC_REVISE_TIME", "ST_SAMPLE_NO,ELM_CODE");
				}
				/////////////end修约update by yiling 20170803

				if ("TQMTS25" == s_table_name)
				{
					if ("1" == s_st_sample_div)//QV样中, 对'H,N,O'元素不做判定
					{
						Log::Trace("", __FUNCTION__, " QV样 ");
						if (tqmts02["ELM_CODE"].ToString() == "001" || tqmts02["ELM_CODE"].ToString() == "014" || tqmts02["ELM_CODE"].ToString() == "016")// H|N|O元素 	  
						{
							elm_ok = 9;
						}
					}
					else if ("4" == s_st_sample_div) //气体样中, 对非'H,N,O'元素不做判定
					{
						Log::Trace("", __FUNCTION__, " 气体样 ");
						//ON样、H样：考虑气体区分指示，来决定该成分是否需要判定。
						if (tqmts02["ELM_CODE"].ToString() != "001" && tqmts02["ELM_CODE"].ToString() != "014" && tqmts02["ELM_CODE"].ToString() != "016")// H|N|O元素 	  
						{
							elm_ok = 9;
						}
						switch (CDecimal::Parse(s_gas_type_div).ToInt16())
						{
							//GAS_TYPE_DIV:1:ON样 2:H样
						case 1: //进行ON样判定
							Log::Trace("", __FUNCTION__, " ON样 ");
							if (tqmts02["ELM_CODE"].ToString() == "014" && s_n_gas_div != "1")
							{
								elm_ok = 9;
							}
							else if (tqmts02["ELM_CODE"].ToString() == "016" && s_o_gas_div != "1")
							{
								elm_ok = 9;
							}
							else if (tqmts02["ELM_CODE"].ToString() == "001" && s_h_gas_div != "1")//update bu zouyonghao 20230607 对ONH进行判定
							{
								elm_ok = 9;
							}
							break;
						case 2: //进行H样判定
							Log::Trace("", __FUNCTION__, " H样 ");
							if (tqmts02["ELM_CODE"].ToString() == "001" && s_h_gas_div != "1")
							{
								elm_ok = 9;
							}
							break;
						default:
							break;
						}
					}
				}
				//未有判定结果的(即无特殊要求，走正常判定流程的)
				if (elm_ok == 0)  // 0:未判定
				{
					//add by reason 2007-3-13
					//要求判定
					//1.标准值为0～0则，判混杂元素  混杂元素管理区分在制造标准中:有值,则判; 无值,??????。
					//2.工序成分的判定
					if (tqmts02["MAIN_MIN"].ToDecimal() == 0 && tqmts02["MAIN_MAX"].ToDecimal() == 0) //标准值为(0,0),则判混杂元素
					{
						Log::Trace("", __FUNCTION__, "判混杂元素tqmts09.hzgl=[{0}]", tqmts09["HZGL"].ToString());
						if (" " == tqmts09["HZGL"].ToString().TrimOrBlank()) //管理区分无, 则判该元素不合, 用以提示用户该出钢记号没有指定混杂判定号
						{
							//没必要去判成分数据，标准值为(0,0)肯定不合
							elm_ok = 2;
						}
						else
						{
							Log::Trace("", __FUNCTION__, " 混杂元素判定 ");
							//元素代码有2个为字符的，无法转换成数字，导致报错 HYF 20130407 CDecimal::Parse(tqmts02["ELM_CODE"].ToString()).ToInt16()
							//增加一段对代码的数字判断，非数字的就不走SWITCH语句了
							isDec = true;
							for (int i = 0; i < strlen(tqmts02["ELM_CODE"].ToString()); i++)
							{
								if (!isdigit(tqmts02["ELM_CODE"].ToString()[i]))
								{
									Log::Trace("", __FUNCTION__, "tqmts02.ELM_CODE=[{0}]", tqmts02["ELM_CODE"].ToString());
									isDec = false;
								}
							}
							Log::Trace("", __FUNCTION__, "123");
							if (isDec)
							{
								Log::Trace("", __FUNCTION__, "混杂元素判定tqmts02.ELM_CODE[{0}]", tqmts02["ELM_CODE"].ToString());
								Log::Trace("", __FUNCTION__, "混杂元素判定elm_act[{0}]", elm_act);
								switch (CDecimal::Parse(tqmts02["ELM_CODE"].ToString()).ToInt16())
								{
								case 64: //铜,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---64-- -tqmts09.CU_MAX[{0}]", tqmts09["CU_MAX"].ToDecimal());
									if (elm_act <= tqmts09["CU_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									cu_ni_cr = cu_ni_cr + elm_act;
									cr_ni_cu_mo = cr_ni_cu_mo + elm_act;
									break;
								case 52: //铬,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---52-- -tqmts09.CR_MAX[{0}]", tqmts09["CR_MAX"].ToDecimal());
									if (elm_act <= tqmts09["CR_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									cu_ni_cr = cu_ni_cr + elm_act;
									cr_ni_cu_mo = cr_ni_cu_mo + elm_act;
									cr_mo = cr_mo + elm_act;
									break;
								case 11: //硼,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---11-- -tqmts09.B_MAX[{0}]", tqmts09["B_MAX"].ToDecimal());
									if (elm_act <= tqmts09["B_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									break;
								case 75: //砷,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---75-- -tqmts09.AS_MAX)[{0}]", tqmts09["AS_MAX"].ToDecimal());
									if (elm_act <= tqmts09["AS_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									break;
								case 96: //钼,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---96-- -tqmts09.MO_MAX[{0}]", tqmts09["MO_MAX"].ToDecimal());
									if (elm_act <= tqmts09["MO_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									break;
									cr_ni_cu_mo = cr_ni_cu_mo + elm_act;
									cr_mo = cr_mo + elm_act;
								case 93: //铌,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---93-- -tqmts09.NB_MAX[{0}]", tqmts09["NB_MAX"].ToDecimal());
									if (elm_act <= tqmts09["NB_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									nb_v_ti = nb_v_ti + elm_act;
									break;
								case 58: //镍,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---58-- -tqmts09.NI_MAX[{0}]", tqmts09["NI_MAX"].ToDecimal());
									if (elm_act <= tqmts09["NI_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									cu_ni_cr = cu_ni_cr + elm_act;
									cr_ni_cu_mo = cr_ni_cu_mo + elm_act;
									break;
								case 119: //锡,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---119-- -tqmts09.SN_MAX[{0}]", tqmts09["SN_MAX"].ToDecimal());
									if (elm_act <= tqmts09["SN_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									break;
								case 48: //钛,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---48-- -tqmts09.TI_MAX[{0}]", tqmts09["TI_MAX"].ToDecimal());
									if (elm_act <= tqmts09["TI_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									nb_v_ti = nb_v_ti + elm_act;
									break;
								case 51: //钒,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---51-- -tqmts09.V_MAX[{0}]", tqmts09["V_MAX"].ToDecimal());
									if (elm_act <= tqmts09["V_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									nb_v_ti = nb_v_ti + elm_act;
									break;
								case 207: //铅,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---207-- -tqmts09.PB_MAX[{0}]", tqmts09["PB_MAX"].ToDecimal());
									if (elm_act <= tqmts09["PB_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									break;
								case 65: //锌,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---65-- -tqmts09.ZN_MAX[{0}]", tqmts09["ZN_MAX"].ToDecimal());
									if (elm_act <= tqmts09["ZN_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									break;
								case 209: //铋,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---209-- -tqmts09.BI_MAX[{0}]", tqmts09["BI_MAX"].ToDecimal());
									if (elm_act <= tqmts09["BI_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									break;
								case 122: //锑,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---122-- -tqmts09.SB_MAX[{0}]", tqmts09["SB_MAX"].ToDecimal());
									if (elm_act <= tqmts09["SB_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									break;
								case 91: //锆,不判标准,判混杂元素
									Log::Trace("", __FUNCTION__, "混杂元素判定---91-- -tqmts09.ZR_MAX[{0}]", tqmts09["ZR_MAX"].ToDecimal());
									if (elm_act <= tqmts09["ZR_MAX"].ToDecimal())
									{
										elm_ok = 8;  //合格
									}
									else
									{
										elm_ok = 2;  //不合格
									}
									as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + elm_act;
									break;
								default:
									break;
								}
							}//IF ISDEC
						}//管理区分有,判混杂
						Log::Trace("", __FUNCTION__, "混杂元素判定结果 = [{0}] ", elm_ok);
					}
					else   //不判混杂元素 ,按工序成分标准判定
					{
						Log::Trace("", __FUNCTION__, "主试标准[{0}-{1}],实绩[{2}]", tqmts02["MAIN_MIN"].ToDecimal(), tqmts02["MAIN_MAX"].ToDecimal(), elm_act);
						if (elm_act >= tqmts02["MAIN_MIN"].ToDecimal() &&
							(elm_act < tqmts02["MAIN_MAX"].ToDecimal() || (fabs(tqmts02["MAIN_MAX"].ToDecimal().ToDouble() - elm_act.ToDouble()) <= 0.000009)))//主试合
						{
							elm_ok = 8;
						}
						else //主试不合
						{
							if (tqmts02["SPE_MAX"].ToDecimal() >= 99.999)//主试不合且无特采要求
							{
								if (elm_act < tqmts02["MAIN_MIN"].ToDecimal())//实际值小于主试最小值
								{
									elm_ok = 2;
								}
								else  //实际值大于主试最大值
								{
									elm_ok = 2;
								}

							}
							else
							{
								if (elm_act >= tqmts02["SPE_MIN"].ToDecimal() &&
									(elm_act <= tqmts02["SPE_MAX"].ToDecimal() || (fabs(tqmts02["SPE_MAX"].ToDecimal().ToDouble() - elm_act.ToDouble()) <= 0.000009)))//主试不合但特采合
								{
									elm_ok = 3;
								}
								else  //主试不合且特采不合
								{
									elm_ok = 1;
								}
							}//特采要求
						}//主试判断
					}////不判混杂元素 ,按工序成分标准判定 end
				}
				Log::Trace("", __FUNCTION__, " 判定结果 = [{0}] ", elm_ok);
			}//有实绩
			else //没有实绩
			{
				Log::Trace("", __FUNCTION__, " 没有实绩 ");
				if ("TQMTS25" == s_table_name)
				{
					if ("1" == s_st_sample_div)//QV样中, 对'H,N,O'元素不做判定
					{
						Log::Trace("", __FUNCTION__, " QV样 ");
						if (tqmts02["ELM_CODE"].ToString() == "001" || tqmts02["ELM_CODE"].ToString() == "014" || tqmts02["ELM_CODE"].ToString() == "016")// H|N|O元素 	  
						{
							elm_ok = 9;
						}
					}
					else if ("4" == s_st_sample_div) //气体样中, 对非'H,N,O'元素不做判定
					{
						Log::Trace("", __FUNCTION__, " 气体样 ");
						//ON样、H样：考虑气体区分指示，来决定该成分是否需要判定。
						if (tqmts02["ELM_CODE"].ToString() != "001" && tqmts02["ELM_CODE"].ToString() != "014" && tqmts02["ELM_CODE"].ToString() != "016")// H|N|O元素 	  
						{
							elm_ok = 9;
						}
						if (s_gas_type_div == "1")////////update by yiling 20180906  GAS_TYPE_DIV:1:ON样 2:H样,
						{
							//if (tqmts02["ELM_CODE"].ToString() == "001")  ///ON样则H元素默认合格
							//{
							//	elm_ok = 9;
							//}
						}
						else if (s_gas_type_div == "2")  //// H样则ON默认合格
						{
							if (tqmts02["ELM_CODE"].ToString() == "014" || tqmts02["ELM_CODE"].ToString() == "016")
							{
								elm_ok = 9;
							}
						}  ////////update by yiling 20180906
						switch (CDecimal::Parse(s_gas_type_div).ToInt16())
						{
							//GAS_TYPE_DIV:1:ON样 2:H样
						case 1: //进行ON样判定
							Log::Trace("", __FUNCTION__, " ON样 ");
							if (tqmts02["ELM_CODE"].ToString() == "014" && s_n_gas_div != "1")
							{
								elm_ok = 9;
							}
							else if (tqmts02["ELM_CODE"].ToString() == "016" && s_o_gas_div != "1")
							{
								elm_ok = 9;
							}
							else if (tqmts02["ELM_CODE"].ToString() == "001" && s_h_gas_div != "1")
							{
								elm_ok = 9;
							}
							break;
						case 2: //进行H样判定
							Log::Trace("", __FUNCTION__, " H样 ");
							if (tqmts02["ELM_CODE"].ToString() == "001" && s_h_gas_div != "1")
							{
								elm_ok = 9;
							}
							break;
						default:
							break;
						}
					}
				}
				//未有判定结果的(即无特殊要求，走正常判定流程的)
				if (elm_ok == 0)  // 0:未判定
				{
					if (tqmts02["MAIN_MIN"].ToDecimal() == 0 && tqmts02["MAIN_MAX"].ToDecimal() >= 99.999)
						elm_ok = 8;//标准无要求，没有实绩判成合格
					else
						elm_ok = 1;//标准有要求，没有实绩判成不合格
				}
				Log::Trace("", __FUNCTION__, " 判定结果 = [{0}] ", elm_ok);
			}//没有实绩
			cmd_inq.Close();

			if (0 == elm_ok)
				elm_0_num++;
			else if (1 == elm_ok || 2 == elm_ok || 3 == elm_ok || 4 == elm_ok)
				elm_1_num++;


			//更新成分合否标志
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE " + s_table_name +
					"    SET ELM_OK = @elm_ok "
					"  WHERE ELM_CODE = @elm_code "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}

			Log::Trace("", "", "UPD---------sqlstr[{0}]", sqlstr);

			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.Parameters.Set("elm_ok", elm_ok);

			Log::Trace("", "", "把判定结果写入TQMTS25表：s_heat_no[{0}], s_st_sample_no[{1}], tqmts02.ELM_CODE[{2}], elm_ok[{3}]", s_heat_no, s_st_sample_no, tqmts02["ELM_CODE"].ToString(), elm_ok);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}// for i

		/*------------------------  试样总判定  ------------------------*/
		Log::Trace("", __FUNCTION__, " elm_0_num /elm_1_num  [{0}]:[{1}] ", elm_0_num, elm_1_num);
		if (elm_0_num > 0)
		{
			s_judge_code = "0";//未判定
		}
		else if (elm_1_num > 0)
		{
			s_judge_code = "2";//不合格
		}
		else
		{
			s_judge_code = "1";//合格
		}

		//add by reason 2007-3-13
		//判定混杂元素，组合不合格把每个元素都置成不合

		Log::Trace("", __FUNCTION__, " cu_ni_cr[{0}]", cu_ni_cr);
		Log::Trace("", __FUNCTION__, " tqmts09.CU_NI_CR[{0}]tqmts09.HZGL[{1}]", tqmts09["CU_NI_CR"].ToDecimal(), tqmts09["HZGL"].ToString());

		if (cu_ni_cr > tqmts09["CU_NI_CR"].ToDecimal() && tqmts09["HZGL"].ToString().TrimOrBlank() != " ")
		{
			s_judge_code = "2";//不合格
			//更新成分合否标志
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE " + s_table_name +
					"    SET ELM_OK = 2 "
					"  WHERE ELM_CODE IN ('052','058','064') "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}
		Log::Trace("", __FUNCTION__, " as_sn_nb_v_ti_zr_b[{0}] tqmts09.AS_SN_NB_V_TI_ZR_B[{1}]", as_sn_nb_v_ti_zr_b, tqmts09["AS_SN_NB_V_TI_ZR_B"].ToDecimal());

		if (as_sn_nb_v_ti_zr_b > tqmts09["AS_SN_NB_V_TI_ZR_B"].ToDecimal() && tqmts09["HZGL"].ToString().TrimOrBlank() != " ")
		{
			s_judge_code = "2";//不合格
			//更新成分合否标志
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE " + s_table_name +
					"    SET ELM_OK = 2 "
					"  WHERE ELM_CODE IN ('011','048','051','075','093','119') "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}
		Log::Trace("", __FUNCTION__, " nb_v_ti[{0}] tqmts09.NB_V_TI[{1}]", nb_v_ti, tqmts09["NB_V_TI"].ToDecimal());

		if (nb_v_ti > tqmts09["NB_V_TI"].ToDecimal() && tqmts09["HZGL"].ToString().TrimOrBlank() != " ")
		{
			s_judge_code = "2";//不合格
			//更新成分合否标志
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE " + s_table_name +
					"    SET ELM_OK = 2 "
					"  WHERE ELM_CODE IN ('048','051','093') "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}
		Log::Trace("", __FUNCTION__, " cr_mo[{0}] tqmts09.CR_MO[{1}]", cr_mo, tqmts09["CR_MO"].ToDecimal());

		if (cr_mo > tqmts09["CR_MO"].ToDecimal() && tqmts09["HZGL"].ToString().TrimOrBlank() != " ")
		{
			s_judge_code = "2";//不合格
			//更新成分合否标志
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE " + s_table_name +
					"    SET ELM_OK = 2 "
					"  WHERE ELM_CODE IN ('052','096') "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}
		Log::Trace("", __FUNCTION__, " cr_ni_cu_mo[{0}] tqmts09.CR_NI_CU_MO[{1}]", cr_ni_cu_mo, tqmts09["CR_NI_CU_MO"].ToDecimal());

		if (cr_ni_cu_mo > tqmts09["CR_NI_CU_MO"].ToDecimal() && tqmts09["HZGL"].ToString().TrimOrBlank() != " ")
		{
			s_judge_code = "2";//不合格
			//更新成分合否标志
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE " + s_table_name +
					"    SET ELM_OK = 2 "
					"  WHERE ELM_CODE IN ('052','058,'064','096') "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}
		/*------------------------  更新 没有判定标准的元素,置其成分合否标志为9  ------------------------*/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE " + s_table_name +
				"    SET ELM_OK = 9 "
				"  WHERE ELM_OK = 0 "
				;
			if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
				" AND ST_SAMPLE_NO = @st_sample_no ";
			if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
			if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("heat_no", s_heat_no);
		cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		/*---------------------  更新总判定结果  ---------------------*/
		if ("TQMTS29" != s_table_name)
		{
			Log::Trace("", __FUNCTION__, "s_judge_code = [{0}]", s_judge_code);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " UPDATE ";
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " TQMTS24 ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " TQMTS2B ";
				sqlstr = sqlstr + "    SET JUDGE_CODE = @judge_code "
					"  WHERE 1 = 1 "
					;
				if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			Log::Trace("", "", "更新总判定结果sqlstr[{0}]", sqlstr);
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("judge_code", s_judge_code);
			cmd_upd.Parameters.Set("heat_no", s_heat_no);
			cmd_upd.Parameters.Set("st_sample_no", s_st_sample_no);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}

		if (!bcls_ret->Tables[0].Columns.Contains("JUDGE_CODE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "JUDGE_CODE");
		}
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["JUDGE_CODE"] = s_judge_code;

		Log::Trace("", __FUNCTION__, "判定成功!");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{

		//CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		//sprintf(s.msg, "sql[%s]erorr[%s]", (const char*)sqlstr, (const char*)ex.GetMsg());
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
		strncpy(s.sysmsg, "System Exception", sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
