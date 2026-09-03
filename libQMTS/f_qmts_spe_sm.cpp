/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2014-08-18
Description: 成分组合元素计算函数（炼钢工序成分收上来时算组合元素）
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




BM2_FUNCTION_EXPORT
int f_qmts_spe_sm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int			doFlag = 0;
	int			i = 0;
	int			fetchRowCount = 0;
	int			pcm_code_Count = 4;
	CString		s_table_name = "";	//表名
	CString		s_heat_no = "";
	CString		s_st_sample_no = "";
	CString		s_pono = "";
	CString		s_rec_creator = s.userid;
	CString		s_rec_create_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString		formula_value = "";	//实绩公式
	CString		formula_value_replace_after = "";	//替换元素后实绩公式
	CString		formula_std = "";	//标准公式
	CString		formula_std_replace_after = "";	//替换元素后标准公式
	CString		symbol = "";	//符号/条件
	CString		symbol_replace_after = "";	//替换元素后符号/条件公式
	CDecimal	symbol_val = 0;	//条件计算结果

	CString		s_elm_code = "";
	CString		s_elm_code_name = "";
	CString		s_elm_name = "";
	CDecimal	s_elm_value = 0;
	CString		elm_null = "";	//公式替换后未找到实绩的元素

	CString		sqlstr = "";
	CString     sqlstr_elm_code = "";
	CString     v_elm_fmla_code = " ";


	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel tqmts25("TQMTS25");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_del(conn);
	CDbCommand cmd_upd(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_qmys(conn);
	CDbCommand cmd_inq_elm(conn);
	CDbCommand cmd_inq_elm_code(conn);
	//输出块
	if (!bcls_ret->Tables[0].Columns.Contains("ELM_CODE"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ELM_CODE");
	}
	if (!bcls_ret->Tables[0].Columns.Contains("MAIN_MIN"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAIN_MIN");
	}
	if (!bcls_ret->Tables[0].Columns.Contains("MAIN_MAX"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAIN_MAX");
	}


	try
	{
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
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank();
		s_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().TrimOrBlank();

		Log::Trace("", __FUNCTION__, "f_qmts_spe IN:---s_table_name		[{0}]", s_table_name);
		Log::Trace("", __FUNCTION__, "f_qmts_spe IN:---s_heat_no			[{0}]", s_heat_no);
		Log::Trace("", __FUNCTION__, "f_qmts_spe IN:---s_st_sample_no		[{0}]", s_st_sample_no);
		Log::Trace("", __FUNCTION__, "f_qmts_spe IN:---tqmts0x.ST_NO		[{0}]", tqmts0x["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_spe IN:---s_pono		[{0}]", s_pono);


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
		if (" " == s_st_sample_no)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数st_sample_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (" " == tqmts0x["ST_NO"].ToString())
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数st_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		//目前组合元素只有CEQ\PCM公式分别对应tqmts0x.ELM_FMLA_CODE1\tqmts0x.ELM_FMLA_CODE2
		//删除 已有组合元素实绩值，重新计算
		Log::Trace("", __FUNCTION__, "DELETE------s_heat_no[{0}]s_st_sample_no[{1}]", s_heat_no, s_st_sample_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " DELETE FROM " + s_table_name +
				"  WHERE SUBSTR(ELM_CODE, 1, 1) BETWEEN 'A' AND 'Z' "
				; 
			if ("TQMTS25" == s_table_name)
			{
				sqlstr = sqlstr + " AND HEAT_NO = @heat_no AND ST_SAMPLE_NO = @st_sample_no";
			}
			if ("TQMTS29" == s_table_name)
			{
				sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
			}
			if ("TQMTS2C" == s_table_name)
			{
				sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
			}
			break;
		}
		cmd_del.SetCommandText(sqlstr);
		cmd_del.Parameters.Set("heat_no", s_heat_no);
		cmd_del.Parameters.Set("st_sample_no", s_st_sample_no);
		cmd_del.ExecuteNonQuery();
		cmd_del.Close();

		Log::Trace("", __FUNCTION__, "UPDATE s_table_name[{0}]sqlstr[{1}]", s_table_name, sqlstr);

		//更新 所有元素,置其初始成分合否标志为0-未判定
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE " + s_table_name +
				"    SET ELM_OK = 0 "
				"  WHERE 1 = 1 "
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

		Log::Trace("", __FUNCTION__, "SELECT  TQMTS0X");

		//获得组合元素公式代码
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr_elm_code = " SELECT ELM_FMLA_CODE1,ELM_FMLA_CODE2,ELM_FMLA_CODE3,ELM_FMLA_CODE4,ELM_FMLA_CODE5,IC_CC_FLAG "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq_elm_code.SetCommandText(sqlstr_elm_code);
		cmd_inq_elm_code.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
		cmd_inq_elm_code.ExecuteReader();
		Log::Trace("", __FUNCTION__, "sqlstr---sqlstr_elm_code[{0}]", sqlstr_elm_code);

		if (cmd_inq_elm_code.Read())
		{
			Log::Trace("", __FUNCTION__, "111111");

			tqmts0x["ELM_FMLA_CODE1"] = cmd_inq_elm_code.GetString(1);
			Log::Trace("", __FUNCTION__, "222222");
			tqmts0x["ELM_FMLA_CODE2"] = cmd_inq_elm_code.GetString(2);
			Log::Trace("", __FUNCTION__, "333333");
			tqmts0x["ELM_FMLA_CODE3"] = cmd_inq_elm_code.GetString(3);
			Log::Trace("", __FUNCTION__, "444444");
			tqmts0x["ELM_FMLA_CODE4"] = cmd_inq_elm_code.GetString(4);
			Log::Trace("", __FUNCTION__, "55555");
			tqmts0x["ELM_FMLA_CODE5"] = cmd_inq_elm_code.GetString(5);
			tqmts0x["IC_CC_FLAG"]	   = cmd_inq_elm_code.GetString(6);
		}
		cmd_inq_elm_code.Close();

		Log::Trace("", __FUNCTION__, "【CEQ】公式代码[{0}], 【PCM】公式代码1[{1}]tqmts0x.IC_CC_FLAG[{2}]", tqmts0x["ELM_FMLA_CODE1"].ToString(), tqmts0x["ELM_FMLA_CODE2"].ToString(), tqmts0x["IC_CC_FLAG"].ToString());
		Log::Trace("", __FUNCTION__, " 【PCM】公式代码2[{0}]【PCM】公式代码3[{1}]【PCM】公式代码4[{2}]【PCM】公式代码5[{3}]", tqmts0x["ELM_FMLA_CODE2"].ToString(), tqmts0x["ELM_FMLA_CODE3"].ToString(), tqmts0x["ELM_FMLA_CODE4"].ToString(), tqmts0x["ELM_FMLA_CODE5"].ToString());


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr_elm_code = " SELECT * "
				"   FROM TQMTS02 "
				"  WHERE ST_NO = @st_no "
				" AND  SUBSTR(ELM_CODE,1,1) BETWEEN 'A' AND 'Z' "
				" AND WHOLE_BACKLOG_CODE ='G' "
				;
			break;
		}
		cmd_inq_elm_code.SetCommandText(sqlstr_elm_code);
		cmd_inq_elm_code.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
		cmd_inq_elm_code.ExecuteReader();
		Log::Trace("", __FUNCTION__, "sqlstr---sqlstr_elm_code[{0}]", sqlstr_elm_code);

		while (cmd_inq_elm_code.Read())
		{
			v_elm_fmla_code = " ";
			cmd_inq_elm_code.Fetch(tqmts02);
			Log::Trace("", __FUNCTION__, "tqmts02.ELM_CODE[{0}]tqmts02.ELM_NAME[{1}]", tqmts02["ELM_CODE"].ToString(), tqmts02["ELM_NAME"].ToString());
			if (tqmts02["ELM_CODE"].ToString().Substring(0, 1) == "C")//碳当量和PCM的计算公式取工艺卡，其他组合元素计算公式直接从QMYS代码描述中获取
			{
				if (tqmts02["ELM_CODE"].ToString().TrimOrBlank() == "C03")//pcm
					v_elm_fmla_code = tqmts0x["ELM_FMLA_CODE2"];
				else//CEQ,ce,cev
					v_elm_fmla_code = tqmts0x["ELM_FMLA_CODE1"];
			}
			else
			{
				tep0002["CODE_CLASS"] = "QMYS";
				tep0002["CODE"] = tqmts02["ELM_CODE"];
				tep0002.Query("CODE_CLASS,CODE");
				Log::Trace("", "", "tep0002.CODE_DESC_5_CONTENT[{0}]", tep0002["CODE_DESC_5_CONTENT"].ToString());
				v_elm_fmla_code = tep0002["CODE_DESC_5_CONTENT"];
			}
			Log::Trace("", "", "v_elm_fmla_code[{0}]", v_elm_fmla_code);
			if (v_elm_fmla_code.TrimOrBlank() != " " && v_elm_fmla_code.TrimOrBlank() != "00")
			{
				//获得组合元素【CEQ】公式代码
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT "
						"   FROM TEP0002 "
						"  WHERE CODE_CLASS = 'QM63' "
						"    AND CODE = @code ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("code", v_elm_fmla_code.Trim());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					formula_value = cmd_inq.GetString(1);
					symbol = cmd_inq.GetString(2);
					formula_std = cmd_inq.GetString(3);

					Log::Trace("", __FUNCTION__, "符号[{0}], 实绩公式[{1}], 标准公式[{1}]", symbol, formula_value, formula_std);

					if (formula_value.TrimOrBlank() != " ")
					{
						Log::Trace("", __FUNCTION__, "实绩公式formula_value 计算开始");
						Log::Trace("", __FUNCTION__, "转换绝对值符号");

						double j = 0;

						for (i = 1; i <= formula_value.GetLength(); i++)
						{
							if ("|" == formula_value.SubstringNE(i - 1, 1))
							{
								j = j + 1;

								Log::Trace("", __FUNCTION__, "实绩公式第i[{0}]位, 第j[{1}]个'|'", i, j);

								if (0 == fmod(j, 2))
								{
									Log::Trace("", __FUNCTION__, "双数");


									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM  SYSIBM.DUAL ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1)  FROM  DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1)  FROM  SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);
									cmd_inq_01.Parameters.Set("formula_value", formula_value);
									cmd_inq_01.Parameters.Set("i", i);
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_value = cmd_inq_01.GetString(1);
									}
									cmd_inq_01.Close();

									Log::Trace("", __FUNCTION__, "双数| 实绩公式[{0}]", formula_value);
								}
								else
								{
									Log::Trace("", __FUNCTION__, "单数");

									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);

									cmd_inq_01.Parameters.Set("formula_value", formula_value);
									cmd_inq_01.Parameters.Set("i", i);
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_value = cmd_inq_01.GetString(1);
									}
									cmd_inq_01.Close();

									Log::Trace("", __FUNCTION__, "单数| 实绩公式[{0}]", formula_value);
								}
							}
						}

						Log::Trace("", __FUNCTION__, "碳当量转换绝对值符号后，实绩公式[{0}]", formula_value);

						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:				// MS SQL Server数据库
						case DB_KIND_ORACLE:	        // Oracle 数据库
						default:						// 所有数据库适用，通用SQL语句
							sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
								"   FROM TEP0002 "
								"  WHERE CODE_CLASS = 'QMYS' "
								"    and CODE_DESC_5_CONTENT =' '"//查找非组合元素
								"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
							break;
						}
						cmd_inq_qmys.SetCommandText(sqlstr);
						cmd_inq_qmys.ExecuteReader();
						while (cmd_inq_qmys.Read())
						{
							s_elm_code = cmd_inq_qmys.GetString(1);
							s_elm_code_name = cmd_inq_qmys.GetString(2);

							//Log::Trace("", __FUNCTION__, "*********************************************************************************");
							Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT replace(@formula_value,@elm_name,'') FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr); 
							cmd_inq_01.Parameters.Set("formula_value", formula_value);
							cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
							cmd_inq_01.ExecuteReader();
							Log::Trace("", __FUNCTION__, "替代前formula_value[{0}]", formula_value);
							if (cmd_inq_01.Read())
							{
								formula_value_replace_after = cmd_inq_01.GetString(1);
							}
							cmd_inq_01.Close();
							Log::Trace("", __FUNCTION__, "替代后formula_value_replace_after[{0}]", formula_value_replace_after);


							/*若元素在公式中存在，查找是否存在实绩数据*/
							if (formula_value_replace_after != formula_value)
							{
								switch (conn->DatabaseKind)
								{
								case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
								case DB_KIND_MSSQL:				// MS SQL Server数据库
								case DB_KIND_ORACLE:	        // Oracle 数据库
								default:						// 所有数据库适用，通用SQL语句
									sqlstr = " SELECT ELM_NAME ";
									if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
									if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
									if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
									sqlstr = sqlstr + "   FROM " + s_table_name +
										"  WHERE ELM_CODE = @elm_code ";
									if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
									///	" AND ST_SAMPLE_NO = @st_sample_no "; //////update by yiling 20200217 如果项目上ONH是一条样，用注释的这句。ONH分开样，用下面这句。
									if ("TQMTS25" == s_table_name && s_elm_code != "001" && s_elm_code != "014"  && s_elm_code != "016")
										sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
									if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
									if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
									break;
								}
								cmd_inq_elm.SetCommandText(sqlstr);
								cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
								cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
								cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
								cmd_inq_elm.ExecuteReader();
								if (cmd_inq_elm.Read())
								{
									s_elm_name = cmd_inq_elm.GetString(1);

									Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_name[{1}],elm_value[{2}]", formula_value, s_elm_name, cmd_inq_elm.GetDecimal(2));

									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = "SELECT replace(@formula_value,@elm_name,@elm_value)  FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT replace(@formula_value,@elm_name,@elm_value)  FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT replace(@formula_value,@elm_name,@elm_value)  FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);

									cmd_inq_01.Parameters.Set("formula_value", formula_value);
									cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
									cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_value = cmd_inq_01.GetString(1);
										Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
									}
									cmd_inq_01.Close();
								}
								else
								{
									elm_null = elm_null + "," + s_elm_code_name.Trim();
									Log::Trace("", __FUNCTION__, "1-未找到实绩的元素：elm_null[{0}]", elm_null);

									Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_name[{1}]", formula_value, s_elm_code_name);

									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT replace(@formula_value,@elm_name,@elm_value) FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT replace(@formula_value,@elm_name,@elm_value) FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);

									cmd_inq_01.Parameters.Set("formula_value", formula_value);
									cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
									cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_value = cmd_inq_01.GetString(1);
										Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
									}
									cmd_inq_01.Close();
								}
								cmd_inq_elm.Close();
							}
						}
						cmd_inq_qmys.Close();
						Log::Trace("", __FUNCTION__, "【CEQ】实绩公式(代入值后)[{0}]", (const char*)formula_value);

						if (elm_null.TrimOrBlank() != " ")
						{
							//if (elm_null.SubstringNE(0, 1) == ",")
							//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
							//CFormattable arguments[] = { elm_null.Trim() };
							//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
							//throw CApplicationException(-1, s.msg, log.Location);
						}

						try
						{
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = " SELECT ROUND(" + formula_value + +",5) FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT ROUND(" + formula_value + +",5) FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT ROUND(" + formula_value + +",5) FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								s_elm_value = cmd_inq_01.GetDecimal(1);
							}
							cmd_inq_01.Close();
						}
						catch (const CException& ex)
						{
							cmd_inq_01.Close();
							strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						Log::Trace("", __FUNCTION__, "【CEQ】实绩公式计算出的值[{0}]", s_elm_value);

						if ("Z" == tqmts0x["ELM_FMLA_CODE1"].ToString().SubstringNE(0, 1).ToUpper())
						{
							Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = "SELECT replace(@symbol,'>','-')  FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT replace(@symbol,'>','-')  FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT replace(@symbol,'>','-')  FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.Parameters.Set("symbol", symbol);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								symbol = cmd_inq_01.GetString(1);
								Log::Trace("", __FUNCTION__, "> symbol[{0}]", symbol);
							}
							cmd_inq_01.Close();

						 
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = "SELECT replace(@symbol,'<','+') FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT replace(@symbol,'<','+')  FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT replace(@symbol,'<','+')  FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.Parameters.Set("symbol", symbol);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								symbol = cmd_inq_01.GetString(1);
								Log::Trace("", __FUNCTION__, "< symbol[{0}]", symbol);
							}
							cmd_inq_01.Close();

							if (symbol.Find("+") > 0)
							{
								symbol = "-" + symbol;
							}

							Log::Trace("", __FUNCTION__, "替换后 symbol[{0}]", symbol);

							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
									"   FROM TEP0002 "
									"  WHERE CODE_CLASS = 'QMYS' "
									"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
								break;
							}
							cmd_inq_qmys.SetCommandText(sqlstr);
							cmd_inq_qmys.ExecuteReader();
							while (cmd_inq_qmys.Read())
							{
								s_elm_code = cmd_inq_qmys.GetString(1);
								s_elm_code_name = cmd_inq_qmys.GetString(2);

								//Log::Trace("", __FUNCTION__, "*********************************************************************************");
								//Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

								switch (conn->DatabaseKind)
								{
								case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
									sqlstr = " SELECT replace(@symbol,@elm_name,'') FROM SYSIBM.DUAL  ";
									break;
								case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
								case DB_KIND_MSSQL:				// MS SQL Server数据库
								case DB_KIND_ORACLE:	        // Oracle 数据库
									sqlstr = " SELECT replace(@symbol,@elm_name,'') FROM DUAL ";
									break;
								default:						// 所有数据库适用，通用SQL语句
									sqlstr = " SELECT replace(@symbol,@elm_name,'') FROM SYSIBM.DUAL ";
									break;
								}
								cmd_inq_01.SetCommandText(sqlstr);
								cmd_inq_01.Parameters.Set("symbol", symbol);
								cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
								cmd_inq_01.ExecuteReader();
								//Log::Trace("", __FUNCTION__, "替代前symbol[{0}]", symbol);
								if (cmd_inq_01.Read())
								{
									symbol_replace_after = cmd_inq_01.GetString(1);
								}
								cmd_inq_01.Close();
								//Log::Trace("", __FUNCTION__, "替代后symbol_replace_after[{0}]", symbol_replace_after);


								/*若元素在公式中存在，查找是否存在实绩数据*/
								if (symbol_replace_after != symbol)
								{
									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT ELM_NAME ";
										if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
										if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
										if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
										sqlstr = sqlstr + "   FROM " + s_table_name +
											"  WHERE ELM_CODE = @elm_code ";
										if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
											" AND ST_SAMPLE_NO = @st_sample_no ";
										if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
										if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
										break;
									}
									cmd_inq_elm.SetCommandText(sqlstr);
									cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
									cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
									cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
									cmd_inq_elm.ExecuteReader();
									if (cmd_inq_elm.Read())
									{
										s_elm_name = cmd_inq_elm.GetString(1);

										Log::Trace("", __FUNCTION__, "symbol[{0}],s_elm_name[{1}],elm_value[{2}]", symbol, s_elm_name, cmd_inq_elm.GetDecimal(2));


										switch (conn->DatabaseKind)
										{
										case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
											sqlstr = " SELECT replace(@symbol,@elm_name,@elm_value) FROM SYSIBM.DUAL  ";
											break;
										case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
										case DB_KIND_MSSQL:				// MS SQL Server数据库
										case DB_KIND_ORACLE:	        // Oracle 数据库
											sqlstr = " SELECT replace(@symbol,@elm_name,@elm_value) FROM DUAL ";
											break;
										default:						// 所有数据库适用，通用SQL语句
											sqlstr = " SELECT replace(@symbol,@elm_name,@elm_value) FROM SYSIBM.DUAL ";
											break;
										}
										cmd_inq_01.SetCommandText(sqlstr);
										cmd_inq_01.Parameters.Set("symbol", symbol);
										cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
										cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
										cmd_inq_01.ExecuteReader();
										if (cmd_inq_01.Read())
										{
											symbol = cmd_inq_01.GetString(1);
											Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
										}
										cmd_inq_01.Close();
									}
									else
									{
										elm_null = elm_null + "," + s_elm_code_name.Trim();
										Log::Trace("", __FUNCTION__, "2-未找到实绩的元素：elm_null[{0}]", elm_null);

										Log::Trace("", __FUNCTION__, "symbol[{0}],s_elm_name[{1}]", symbol, s_elm_code_name);

										switch (conn->DatabaseKind)
										{
										case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
											sqlstr = " SELECT replace(@symbol,@elm_name,@elm_value) FROM SYSIBM.DUAL  ";
											break;
										case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
										case DB_KIND_MSSQL:				// MS SQL Server数据库
										case DB_KIND_ORACLE:	        // Oracle 数据库
											sqlstr = " SELECT replace(@symbol,@elm_name,@elm_value) FROM DUAL ";
											break;
										default:						// 所有数据库适用，通用SQL语句
											sqlstr = " SELECT replace(@symbol,@elm_name,@elm_value) FROM SYSIBM.DUAL ";
											break;
										}
										cmd_inq_01.SetCommandText(sqlstr);
										cmd_inq_01.Parameters.Set("symbol", symbol);
										cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
										cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
										cmd_inq_01.ExecuteReader();
										if (cmd_inq_01.Read())
										{
											symbol = cmd_inq_01.GetString(1);
											Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
										}
										cmd_inq_01.Close();
									}
									cmd_inq_elm.Close();
								}
							}
							cmd_inq_qmys.Close();

							Log::Trace("", __FUNCTION__, "symbol公式(代入值后)[{0}]", (const char*)symbol);

							if (elm_null.TrimOrBlank() != " ")
							{
								//if (elm_null.SubstringNE(0, 1) == ",")
								//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
								//CFormattable arguments[] = { elm_null.Trim() };
								//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
								//throw CApplicationException(-1, s.msg, log.Location);
							}
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = " SELECT " + symbol + " FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT " + symbol + " FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT " + symbol + " FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								symbol_val = cmd_inq_01.GetDecimal(1);
							}
							cmd_inq_01.Close();

							Log::Trace("", __FUNCTION__, "条件计算结构 symbol_val[{0}]", symbol_val);
							Log::Trace("", __FUNCTION__, "s_heat_no[{0}]", s_heat_no);
							Log::Trace("", __FUNCTION__, "s_st_sample_no[{0}]", s_st_sample_no);
							Log::Trace("", __FUNCTION__, "tqmts0x.ST_NO[{0}]", tqmts0x["ST_NO"].ToString());
							Log::Trace("", __FUNCTION__, "s_elm_value[{0}]", s_elm_value);

							if (symbol_val >= 0)
							{
								//新增 组合元素实绩值
								switch (conn->DatabaseKind)
								{
								case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
								case DB_KIND_MSSQL:				// MS SQL Server数据库
								case DB_KIND_ORACLE:	        // Oracle 数据库
								default:						// 所有数据库适用，通用SQL语句
									sqlstr = " INSERT INTO " + s_table_name;
									if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_ACT_OLD,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
										" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
									if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
										" VALUES(@pono,@heat_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
									if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " (PONO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_ACT_OLD,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
										" VALUES(@pono,@st_sample_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
									break;
								}
								cmd_ins.SetCommandText(sqlstr);
								cmd_ins.Parameters.Set("pono", s_pono);
								cmd_ins.Parameters.Set("heat_no", s_heat_no);
								cmd_ins.Parameters.Set("st_sample_no", s_st_sample_no);
								cmd_ins.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
								cmd_ins.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
								cmd_ins.Parameters.Set("tqmts02.ELM_NAME", tqmts02["ELM_NAME"].ToString());
								cmd_ins.Parameters.Set("elm_value", s_elm_value);
								cmd_ins.Parameters.Set("rec_creator", "DBCOUNT");
								cmd_ins.Parameters.Set("rec_create_time", s_rec_create_time);
								cmd_ins.Parameters.Set("whole_backlog_code", tqmts0x["IC_CC_FLAG"].ToString());
								cmd_ins.ExecuteNonQuery();
								cmd_ins.Close();
							}
						}
						else
						{
							Log::Trace("", __FUNCTION__, "*********************************************");
							Log::Trace("", __FUNCTION__, "s_heat_no[{0}]", s_heat_no);
							Log::Trace("", __FUNCTION__, "s_st_sample_no[{0}]", s_st_sample_no);
							Log::Trace("", __FUNCTION__, "tqmts0x.ST_NO[{0}]", tqmts0x["ST_NO"].ToString());
							Log::Trace("", __FUNCTION__, "s_elm_value[{0}]", s_elm_value);
							//新增 组合元素实绩值
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " INSERT INTO " + s_table_name;
								if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_ACT_OLD,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
									" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
								if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
									" VALUES(@pono,@heat_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
								if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " (PONO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_ACT_OLD,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
									" VALUES(@pono,@st_sample_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
								break;
							}
							Log::Trace("", __FUNCTION__, "C01--sqlstr[{0}]", sqlstr);
							cmd_ins.SetCommandText(sqlstr);
							cmd_ins.Parameters.Set("pono", s_pono);
							cmd_ins.Parameters.Set("heat_no", s_heat_no);
							cmd_ins.Parameters.Set("st_sample_no", s_st_sample_no);
							cmd_ins.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
							cmd_ins.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
							cmd_ins.Parameters.Set("tqmts02.ELM_NAME", tqmts02["ELM_NAME"].ToString());
							cmd_ins.Parameters.Set("elm_value", s_elm_value);
							cmd_ins.Parameters.Set("rec_creator", "DBCOUNT");
							cmd_ins.Parameters.Set("rec_create_time", s_rec_create_time);
							cmd_ins.Parameters.Set("whole_backlog_code", tqmts0x["IC_CC_FLAG"].ToString());
							cmd_ins.ExecuteNonQuery();
							cmd_ins.Close();
						}
					}

					if (formula_std.TrimOrBlank() != " ")
					{
						Log::Trace("", __FUNCTION__, "标准公式formula_std 计算开始");
						Log::Trace("", __FUNCTION__, "转换绝对值符号");

						double j = 0;

						for (i = 1; i <= formula_std.GetLength(); i++)
						{
							if ("|" == formula_std.SubstringNE(i - 1, 1))
							{
								j = j + 1;

								Log::Trace("", __FUNCTION__, "标准公式第i[{0}]位, 第j[{1}]个'|'", i, j);

								if (0 == fmod(j, 2))
								{
									Log::Trace("", __FUNCTION__, "双数");

									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT substr(@formula_std,1,@i-1) || ')' || substr(@formula_std,@i+1) FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT substr(@formula_std,1,@i-1) || ')' || substr(@formula_std,@i+1) FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT substr(@formula_std,1,@i-1) || ')' || substr(@formula_std,@i+1) FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);
									cmd_inq_01.Parameters.Set("formula_std", formula_std);
									cmd_inq_01.Parameters.Set("i", i);
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_std = cmd_inq_01.GetString(1);
									}
									cmd_inq_01.Close();

									Log::Trace("", __FUNCTION__, "双数| 标准公式[{0}]", formula_std);
								}
								else
								{
									Log::Trace("", __FUNCTION__, "单数");

									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT substr(@formula_std,1,@i-1) || 'abs(' || substr(@formula_std,@i+1) FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT substr(@formula_std,1,@i-1) || 'abs(' || substr(@formula_std,@i+1) FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT substr(@formula_std,1,@i-1) || 'abs(' || substr(@formula_std,@i+1) FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);
									cmd_inq_01.Parameters.Set("formula_std", formula_std);
									cmd_inq_01.Parameters.Set("i", i);
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_std = cmd_inq_01.GetString(1);
									}
									cmd_inq_01.Close();

									Log::Trace("", __FUNCTION__, "单数| 标准公式[{0}]", formula_std);
								}
							}
						}

						Log::Trace("", __FUNCTION__, "转换绝对值符号后，标准公式[{0}]", formula_std);

						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:				// MS SQL Server数据库
						case DB_KIND_ORACLE:	        // Oracle 数据库
						default:						// 所有数据库适用，通用SQL语句
							sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
								"   FROM TEP0002 "
								"  WHERE CODE_CLASS = 'QMYS' "
								"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
							break;
						}
						cmd_inq_qmys.SetCommandText(sqlstr);
						cmd_inq_qmys.ExecuteReader();
						while (cmd_inq_qmys.Read())
						{
							s_elm_code = cmd_inq_qmys.GetString(1);
							s_elm_code_name = cmd_inq_qmys.GetString(2);

							Log::Trace("", __FUNCTION__, "*********************************************************************************");
							Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

						 	switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = " SELECT replace(@formula_std,@elm_name,'') FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT replace(@formula_std,@elm_name,'') FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT replace(@formula_std,@elm_name,'') FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.Parameters.Set("formula_std", formula_std);
							cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
							cmd_inq_01.ExecuteReader();
							Log::Trace("", __FUNCTION__, "替代前formula_std[{0}]", formula_std);
							if (cmd_inq_01.Read())
							{
								formula_std_replace_after = cmd_inq_01.GetString(1);
							}
							cmd_inq_01.Close();
							Log::Trace("", __FUNCTION__, "替代后formula_std_replace_after[{0}]", formula_std_replace_after);


							/*若元素在公式中存在，查找是否存在实绩数据*/
							if (formula_std_replace_after != formula_std)
							{
								switch (conn->DatabaseKind)
								{
								case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
								case DB_KIND_MSSQL:				// MS SQL Server数据库
								case DB_KIND_ORACLE:	        // Oracle 数据库
								default:						// 所有数据库适用，通用SQL语句
									sqlstr = " SELECT ELM_NAME ";
									if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
									if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
									if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
									sqlstr = sqlstr + "   FROM " + s_table_name +
										"  WHERE ELM_CODE = @elm_code ";
									if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
										" AND ST_SAMPLE_NO = @st_sample_no ";
									if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
									if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
									break;
								}
								cmd_inq_elm.SetCommandText(sqlstr);
								cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
								cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
								cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
								cmd_inq_elm.ExecuteReader();
								if (cmd_inq_elm.Read())
								{
									s_elm_name = cmd_inq_elm.GetString(1);

									Log::Trace("", __FUNCTION__, "formula_std[{0}],s_elm_name[{1}],elm_value[{2}]", formula_std, s_elm_name, cmd_inq_elm.GetDecimal(2));

									 
									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT replace(@formula_std,@elm_name,@elm_value) FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT replace(@formula_std,@elm_name,@elm_value) FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT replace(@formula_std,@elm_name,@elm_value) FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);
									cmd_inq_01.Parameters.Set("formula_std", formula_std);
									cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
									cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_std = cmd_inq_01.GetString(1);
										Log::Trace("", __FUNCTION__, "formula_std[{0}]", formula_std);
									}
									cmd_inq_01.Close();
								}
								else
								{
									elm_null = elm_null + "," + s_elm_code_name.Trim();
									Log::Trace("", __FUNCTION__, "3-未找到实绩的元素：elm_null[{0}]", elm_null);

									Log::Trace("", __FUNCTION__, "formula_std[{0}],s_elm_name[{1}]", formula_std, s_elm_code_name);

									 
									switch (conn->DatabaseKind)
									{
									case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
										sqlstr = " SELECT replace(@formula_std,@elm_name,@elm_value) FROM SYSIBM.DUAL  ";
										break;
									case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
									case DB_KIND_MSSQL:				// MS SQL Server数据库
									case DB_KIND_ORACLE:	        // Oracle 数据库
										sqlstr = " SELECT replace(@formula_std,@elm_name,@elm_value) FROM DUAL ";
										break;
									default:						// 所有数据库适用，通用SQL语句
										sqlstr = " SELECT replace(@formula_std,@elm_name,@elm_value) FROM SYSIBM.DUAL ";
										break;
									}
									cmd_inq_01.SetCommandText(sqlstr);
									cmd_inq_01.Parameters.Set("formula_std", formula_std);
									cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
									cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
									cmd_inq_01.ExecuteReader();
									if (cmd_inq_01.Read())
									{
										formula_std = cmd_inq_01.GetString(1);
										Log::Trace("", __FUNCTION__, "formula_std[{0}]", formula_std);
									}
									cmd_inq_01.Close();
								}
								cmd_inq_elm.Close();
							}
						}
						cmd_inq_qmys.Close();
						Log::Trace("", __FUNCTION__, "【CEQ】标准公式(代入值后)[{0}]", (const char*)formula_std);

						if (elm_null.TrimOrBlank() != " ")
						{
							//if (elm_null.SubstringNE(0, 1) == ",")
							//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
							//CFormattable arguments[] = { elm_null.Trim() };
							//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
							//throw CApplicationException(-1, s.msg, log.Location);
						}

						try
						{
							switch (conn->DatabaseKind)
							{
							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								sqlstr = " SELECT ROUND(" + formula_std + +",5) FROM SYSIBM.DUAL  ";
								break;
							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
							case DB_KIND_MSSQL:				// MS SQL Server数据库
							case DB_KIND_ORACLE:	        // Oracle 数据库
								sqlstr = " SELECT ROUND(" + formula_std + +",5) FROM DUAL ";
								break;
							default:						// 所有数据库适用，通用SQL语句
								sqlstr = " SELECT ROUND(" + formula_std + +",5) FROM SYSIBM.DUAL ";
								break;
							}
							cmd_inq_01.SetCommandText(sqlstr);
							cmd_inq_01.ExecuteReader();
							if (cmd_inq_01.Read())
							{
								s_elm_value = cmd_inq_01.GetDecimal(1);
							}
							cmd_inq_01.Close();
						}
						catch (const CException& ex)
						{
							cmd_inq_01.Close();
							strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						Log::Trace("", __FUNCTION__, "【CEQ】标准公式计算出的值[{0}]", s_elm_value);

						CDataRow& row = bcls_ret->Tables[0].Rows.Add();
						row["ELM_CODE"] = "C01";
						//row["MAIN_MIN"] = 0;
						//row["MAIN_MAX"] = 9999.999999;
						if (symbol == "<")
						{
							bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = s_elm_value;
							Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的最大值[{0}]", s_elm_value);
						}
						else if (symbol == ">")
						{
							bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = s_elm_value;
							Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的最小值[{0}]", s_elm_value);
						}
					}
				}
				cmd_inq.Close();
			}
		}
		
		cmd_inq_elm_code.Close();
		

		

		////【CEQ】公式计算
		//if (tqmts0x["ELM_FMLA_CODE1"].ToString().TrimOrBlank() != " " && tqmts0x["ELM_FMLA_CODE1"].ToString().TrimOrBlank() != "00")
		//{
		//	//获得组合元素【CEQ】公式代码
		//	switch (conn->DatabaseKind)
		//	{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:						// 所有数据库适用，通用SQL语句
		//		sqlstr = " SELECT CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT "
		//			"   FROM TEP0002 "
		//			"  WHERE CODE_CLASS = 'QM63' "
		//			"    AND CODE = @code ";
		//		break;
		//	}
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("code", tqmts0x["ELM_FMLA_CODE1"].ToString().Trim());
		//	cmd_inq.ExecuteReader();
		//	if (cmd_inq.Read())
		//	{
		//		formula_value = cmd_inq.GetString(1);
		//		symbol = cmd_inq.GetString(2);
		//		formula_std = cmd_inq.GetString(3);

		//		Log::Trace("", __FUNCTION__, "符号[{0}], 实绩公式[{1}], 标准公式[{1}]", symbol, formula_value, formula_std);

		//		if (formula_value.TrimOrBlank() != " ")
		//		{
		//			Log::Trace("", __FUNCTION__, "实绩公式formula_value 计算开始");
		//			Log::Trace("", __FUNCTION__, "转换绝对值符号");

		//			double j = 0;

		//			for (i = 1; i <= formula_value.GetLength(); i++)
		//			{
		//				if ("|" == formula_value.SubstringNE(i - 1, 1))
		//				{
		//					j = j + 1;

		//					Log::Trace("", __FUNCTION__, "实绩公式第i[{0}]位, 第j[{1}]个'|'", i, j);

		//					if (0 == fmod(j, 2))
		//					{
		//						Log::Trace("", __FUNCTION__, "双数");

		//						cmd_inq_01.SetCommandText(" SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//						cmd_inq_01.Parameters.Set("i", i);
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_value = cmd_inq_01.GetString(1);
		//						}
		//						cmd_inq_01.Close();

		//						Log::Trace("", __FUNCTION__, "双数| 实绩公式[{0}]", formula_value);
		//					}
		//					else
		//					{
		//						Log::Trace("", __FUNCTION__, "单数");

		//						cmd_inq_01.SetCommandText(" SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//						cmd_inq_01.Parameters.Set("i", i);
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_value = cmd_inq_01.GetString(1);
		//						}
		//						cmd_inq_01.Close();

		//						Log::Trace("", __FUNCTION__, "单数| 实绩公式[{0}]", formula_value);
		//					}
		//				}
		//			}

		//			Log::Trace("", __FUNCTION__, "碳当量转换绝对值符号后，实绩公式[{0}]", formula_value);

		//			switch (conn->DatabaseKind)
		//			{
		//			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//			case DB_KIND_MSSQL:				// MS SQL Server数据库
		//			case DB_KIND_ORACLE:	        // Oracle 数据库
		//			default:						// 所有数据库适用，通用SQL语句
		//				sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
		//					"   FROM TEP0002 "
		//					"  WHERE CODE_CLASS = 'QMYS' "
		//					"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
		//				break;
		//			}
		//			cmd_inq_qmys.SetCommandText(sqlstr);
		//			cmd_inq_qmys.ExecuteReader();
		//			while (cmd_inq_qmys.Read())
		//			{
		//				s_elm_code = cmd_inq_qmys.GetString(1);
		//				s_elm_code_name = cmd_inq_qmys.GetString(2);

		//				//Log::Trace("", __FUNCTION__, "*********************************************************************************");
		//				Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

		//				cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,'') FROM DUAL ");
		//				cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//				cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//				cmd_inq_01.ExecuteReader();
		//				Log::Trace("", __FUNCTION__, "替代前formula_value[{0}]", formula_value);
		//				if (cmd_inq_01.Read())
		//				{
		//					formula_value_replace_after = cmd_inq_01.GetString(1);
		//				}
		//				cmd_inq_01.Close();
		//				Log::Trace("", __FUNCTION__, "替代后formula_value_replace_after[{0}]", formula_value_replace_after);


		//				/*若元素在公式中存在，查找是否存在实绩数据*/
		//				if (formula_value_replace_after != formula_value)
		//				{
		//					switch (conn->DatabaseKind)
		//					{
		//					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//					case DB_KIND_MSSQL:				// MS SQL Server数据库
		//					case DB_KIND_ORACLE:	        // Oracle 数据库
		//					default:						// 所有数据库适用，通用SQL语句
		//						sqlstr = " SELECT ELM_NAME ";
		//						if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
		//						if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//						if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//						sqlstr = sqlstr + "   FROM " + s_table_name +
		//							"  WHERE ELM_CODE = @elm_code ";
		//						if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
		//							" AND ST_SAMPLE_NO = @st_sample_no ";
		//						if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
		//						if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
		//						break;
		//					}
		//					cmd_inq_elm.SetCommandText(sqlstr);
		//					cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
		//					cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
		//					cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
		//					cmd_inq_elm.ExecuteReader();
		//					if (cmd_inq_elm.Read())
		//					{
		//						s_elm_name = cmd_inq_elm.GetString(1);

		//						Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_name[{1}],elm_value[{2}]", formula_value, s_elm_name, cmd_inq_elm.GetDecimal(2));

		//						cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//						cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
		//						cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_value = cmd_inq_01.GetString(1);
		//							Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
		//						}
		//						cmd_inq_01.Close();
		//					}
		//					else
		//					{
		//						elm_null = elm_null + "," + s_elm_code_name.Trim();
		//						Log::Trace("", __FUNCTION__, "1-未找到实绩的元素：elm_null[{0}]", elm_null);

		//						Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_name[{1}]", formula_value, s_elm_code_name);

		//						cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//						cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//						cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_value = cmd_inq_01.GetString(1);
		//							Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
		//						}
		//						cmd_inq_01.Close();
		//					}
		//					cmd_inq_elm.Close();
		//				}
		//			}
		//			cmd_inq_qmys.Close();
		//			Log::Trace("", __FUNCTION__, "【CEQ】实绩公式(代入值后)[{0}]", (const char*)formula_value);

		//			if (elm_null.TrimOrBlank() != " ")
		//			{
		//				//if (elm_null.SubstringNE(0, 1) == ",")
		//				//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
		//				//CFormattable arguments[] = { elm_null.Trim() };
		//				//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
		//				//throw CApplicationException(-1, s.msg, log.Location);
		//			}

		//			try
		//			{
		//				cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_value + +",2) FROM DUAL ");
		//				cmd_inq_01.ExecuteReader();
		//				if (cmd_inq_01.Read())
		//				{
		//					s_elm_value = cmd_inq_01.GetDecimal(1);
		//				}
		//				cmd_inq_01.Close();
		//			}
		//			catch (const CException& ex)
		//			{
		//				cmd_inq_01.Close();
		//				strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
		//				throw CApplicationException(-1, s.msg, log.Location);
		//			}
		//			Log::Trace("", __FUNCTION__, "【CEQ】实绩公式计算出的值[{0}]", s_elm_value);

		//			if ("Z" == tqmts0x["ELM_FMLA_CODE1"].ToString().SubstringNE(0, 1).ToUpper())
		//			{
		//				Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);

		//				cmd_inq_01.SetCommandText(" SELECT replace(@symbol,'>','-') FROM DUAL ");
		//				cmd_inq_01.Parameters.Set("symbol", symbol);
		//				cmd_inq_01.ExecuteReader();
		//				if (cmd_inq_01.Read())
		//				{
		//					symbol = cmd_inq_01.GetString(1);
		//					Log::Trace("", __FUNCTION__, "> symbol[{0}]", symbol);
		//				}
		//				cmd_inq_01.Close();

		//				cmd_inq_01.SetCommandText(" SELECT replace(@symbol,'<','+') FROM DUAL ");
		//				cmd_inq_01.Parameters.Set("symbol", symbol);
		//				cmd_inq_01.ExecuteReader();
		//				if (cmd_inq_01.Read())
		//				{
		//					symbol = cmd_inq_01.GetString(1);
		//					Log::Trace("", __FUNCTION__, "< symbol[{0}]", symbol);
		//				}
		//				cmd_inq_01.Close();

		//				if (symbol.Find("+") > 0)
		//				{
		//					symbol = "-" + symbol;
		//				}

		//				Log::Trace("", __FUNCTION__, "替换后 symbol[{0}]", symbol);

		//				switch (conn->DatabaseKind)
		//				{
		//				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//				case DB_KIND_MSSQL:				// MS SQL Server数据库
		//				case DB_KIND_ORACLE:	        // Oracle 数据库
		//				default:						// 所有数据库适用，通用SQL语句
		//					sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
		//						"   FROM TEP0002 "
		//						"  WHERE CODE_CLASS = 'QMYS' "
		//						"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
		//					break;
		//				}
		//				cmd_inq_qmys.SetCommandText(sqlstr);
		//				cmd_inq_qmys.ExecuteReader();
		//				while (cmd_inq_qmys.Read())
		//				{
		//					s_elm_code = cmd_inq_qmys.GetString(1);
		//					s_elm_code_name = cmd_inq_qmys.GetString(2);

		//					//Log::Trace("", __FUNCTION__, "*********************************************************************************");
		//					//Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

		//					cmd_inq_01.SetCommandText(" SELECT replace(@symbol,@elm_name,'') FROM DUAL ");
		//					cmd_inq_01.Parameters.Set("symbol", symbol);
		//					cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//					cmd_inq_01.ExecuteReader();
		//					//Log::Trace("", __FUNCTION__, "替代前symbol[{0}]", symbol);
		//					if (cmd_inq_01.Read())
		//					{
		//						symbol_replace_after = cmd_inq_01.GetString(1);
		//					}
		//					cmd_inq_01.Close();
		//					//Log::Trace("", __FUNCTION__, "替代后symbol_replace_after[{0}]", symbol_replace_after);


		//					/*若元素在公式中存在，查找是否存在实绩数据*/
		//					if (symbol_replace_after != symbol)
		//					{
		//						switch (conn->DatabaseKind)
		//						{
		//						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//						case DB_KIND_MSSQL:				// MS SQL Server数据库
		//						case DB_KIND_ORACLE:	        // Oracle 数据库
		//						default:						// 所有数据库适用，通用SQL语句
		//							sqlstr = " SELECT ELM_NAME ";
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//							sqlstr = sqlstr + "   FROM " + s_table_name +
		//								"  WHERE ELM_CODE = @elm_code ";
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
		//								" AND ST_SAMPLE_NO = @st_sample_no ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
		//							break;
		//						}
		//						cmd_inq_elm.SetCommandText(sqlstr);
		//						cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
		//						cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
		//						cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
		//						cmd_inq_elm.ExecuteReader();
		//						if (cmd_inq_elm.Read())
		//						{
		//							s_elm_name = cmd_inq_elm.GetString(1);

		//							Log::Trace("", __FUNCTION__, "symbol[{0}],s_elm_name[{1}],elm_value[{2}]", symbol, s_elm_name, cmd_inq_elm.GetDecimal(2));

		//							cmd_inq_01.SetCommandText(" SELECT replace(@symbol,@elm_name,@elm_value) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("symbol", symbol);
		//							cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
		//							cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								symbol = cmd_inq_01.GetString(1);
		//								Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
		//							}
		//							cmd_inq_01.Close();
		//						}
		//						else
		//						{
		//							elm_null = elm_null + "," + s_elm_code_name.Trim();
		//							Log::Trace("", __FUNCTION__, "2-未找到实绩的元素：elm_null[{0}]", elm_null);

		//							Log::Trace("", __FUNCTION__, "symbol[{0}],s_elm_name[{1}]", symbol, s_elm_code_name);

		//							cmd_inq_01.SetCommandText(" SELECT replace(@symbol,@elm_name,@elm_value) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("symbol", symbol);
		//							cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//							cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								symbol = cmd_inq_01.GetString(1);
		//								Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
		//							}
		//							cmd_inq_01.Close();
		//						}
		//						cmd_inq_elm.Close();
		//					}
		//				}
		//				cmd_inq_qmys.Close();

		//				Log::Trace("", __FUNCTION__, "symbol公式(代入值后)[{0}]", (const char*)symbol);

		//				if (elm_null.TrimOrBlank() != " ")
		//				{
		//					//if (elm_null.SubstringNE(0, 1) == ",")
		//					//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
		//					//CFormattable arguments[] = { elm_null.Trim() };
		//					//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
		//					//throw CApplicationException(-1, s.msg, log.Location);
		//				}

		//				cmd_inq_01.SetCommandText(" SELECT " + symbol + " FROM DUAL ");
		//				cmd_inq_01.ExecuteReader();
		//				if (cmd_inq_01.Read())
		//				{
		//					symbol_val = cmd_inq_01.GetDecimal(1);
		//				}
		//				cmd_inq_01.Close();

		//				Log::Trace("", __FUNCTION__, "条件计算结构 symbol_val[{0}]", symbol_val);
		//				Log::Trace("", __FUNCTION__, "s_heat_no[{0}]", s_heat_no);
		//				Log::Trace("", __FUNCTION__, "s_st_sample_no[{0}]", s_st_sample_no);
		//				Log::Trace("", __FUNCTION__, "tqmts0x.ST_NO[{0}]", tqmts0x["ST_NO"].ToString());
		//				Log::Trace("", __FUNCTION__, "s_elm_value[{0}]", s_elm_value);

		//				if (symbol_val >= 0)
		//				{
		//					//新增 组合元素实绩值
		//					switch (conn->DatabaseKind)
		//					{
		//					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//					case DB_KIND_MSSQL:				// MS SQL Server数据库
		//					case DB_KIND_ORACLE:	        // Oracle 数据库
		//					default:						// 所有数据库适用，通用SQL语句
		//						sqlstr = " INSERT INTO " + s_table_name;
		//						if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//							" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@tqmts02["ELM_CODE"].ToString(),@tqmts02["ELM_NAME"].ToString(),@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
		//						if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//							" VALUES(@pono,@heat_no,@st_no,@tqmts02["ELM_CODE"].ToString(),@tqmts02["ELM_NAME"].ToString(),@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
		//						if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " (PONO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//							" VALUES(@pono,@st_sample_no,@st_no,@tqmts02["ELM_CODE"].ToString(),@tqmts02["ELM_NAME"].ToString(),@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
		//						break;
		//					}
		//					cmd_ins.SetCommandText(sqlstr);
		//					cmd_ins.Parameters.Set("pono", s_pono);
		//					cmd_ins.Parameters.Set("heat_no", s_heat_no);
		//					cmd_ins.Parameters.Set("st_sample_no", s_st_sample_no);
		//					cmd_ins.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
		//					cmd_ins.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
		//					cmd_ins.Parameters.Set("tqmts02.ELM_NAME", tqmts02["ELM_NAME"].ToString());
		//					cmd_ins.Parameters.Set("elm_value", s_elm_value);
		//					cmd_ins.Parameters.Set("rec_creator", "DBCOUNT");
		//					cmd_ins.Parameters.Set("rec_create_time", s_rec_create_time);
		//					cmd_ins.Parameters.Set("whole_backlog_code", "C");
		//					cmd_ins.ExecuteNonQuery();
		//					cmd_ins.Close();
		//				}
		//			}
		//			else
		//			{
		//				Log::Trace("", __FUNCTION__, "*********************************************");
		//				Log::Trace("", __FUNCTION__, "s_heat_no[{0}]", s_heat_no);
		//				Log::Trace("", __FUNCTION__, "s_st_sample_no[{0}]", s_st_sample_no);
		//				Log::Trace("", __FUNCTION__, "tqmts0x.ST_NO[{0}]", tqmts0x["ST_NO"].ToString());
		//				Log::Trace("", __FUNCTION__, "s_elm_value[{0}]", s_elm_value);
		//				//新增 组合元素实绩值
		//				switch (conn->DatabaseKind)
		//				{
		//				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//				case DB_KIND_MSSQL:				// MS SQL Server数据库
		//				case DB_KIND_ORACLE:	        // Oracle 数据库
		//				default:						// 所有数据库适用，通用SQL语句
		//					sqlstr = " INSERT INTO " + s_table_name;
		//					if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//						" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@tqmts02["ELM_CODE"].ToString(),@tqmts02["ELM_NAME"].ToString(),@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//					if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//						" VALUES(@pono,@heat_no,@st_no,@tqmts02["ELM_CODE"].ToString(),@tqmts02["ELM_NAME"].ToString(),@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//					if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " (PONO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//						" VALUES(@pono,@st_sample_no,@st_no,@tqmts02["ELM_CODE"].ToString(),@tqmts02["ELM_NAME"].ToString(),@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//					break;
		//				}
		//				Log::Trace("", __FUNCTION__, "C01--sqlstr[{0}]", sqlstr);
		//				cmd_ins.SetCommandText(sqlstr);
		//				cmd_ins.Parameters.Set("pono", s_pono);
		//				cmd_ins.Parameters.Set("heat_no", s_heat_no);
		//				cmd_ins.Parameters.Set("st_sample_no", s_st_sample_no);
		//				cmd_ins.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
		//				cmd_ins.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
		//				cmd_ins.Parameters.Set("tqmts02.ELM_NAME", tqmts02["ELM_NAME"].ToString());
		//				cmd_ins.Parameters.Set("elm_value", s_elm_value);
		//				cmd_ins.Parameters.Set("rec_creator", "DBCOUNT");
		//				cmd_ins.Parameters.Set("rec_create_time", s_rec_create_time);
		//				cmd_ins.Parameters.Set("whole_backlog_code", "C");
		//				cmd_ins.ExecuteNonQuery();
		//				cmd_ins.Close();
		//			}
		//		}

		//		if (formula_std.TrimOrBlank() != " ")
		//		{
		//			Log::Trace("", __FUNCTION__, "标准公式formula_std 计算开始");
		//			Log::Trace("", __FUNCTION__, "转换绝对值符号");

		//			double j = 0;

		//			for (i = 1; i <= formula_std.GetLength(); i++)
		//			{
		//				if ("|" == formula_std.SubstringNE(i - 1, 1))
		//				{
		//					j = j + 1;

		//					Log::Trace("", __FUNCTION__, "标准公式第i[{0}]位, 第j[{1}]个'|'", i, j);

		//					if (0 == fmod(j, 2))
		//					{
		//						Log::Trace("", __FUNCTION__, "双数");

		//						cmd_inq_01.SetCommandText(" SELECT substr(@formula_std,1,@i-1) || ')' || substr(@formula_std,@i+1) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//						cmd_inq_01.Parameters.Set("i", i);
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_std = cmd_inq_01.GetString(1);
		//						}
		//						cmd_inq_01.Close();

		//						Log::Trace("", __FUNCTION__, "双数| 标准公式[{0}]", formula_std);
		//					}
		//					else
		//					{
		//						Log::Trace("", __FUNCTION__, "单数");

		//						cmd_inq_01.SetCommandText(" SELECT substr(@formula_std,1,@i-1) || 'abs(' || substr(@formula_std,@i+1) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//						cmd_inq_01.Parameters.Set("i", i);
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_std = cmd_inq_01.GetString(1);
		//						}
		//						cmd_inq_01.Close();

		//						Log::Trace("", __FUNCTION__, "单数| 标准公式[{0}]", formula_std);
		//					}
		//				}
		//			}

		//			Log::Trace("", __FUNCTION__, "转换绝对值符号后，标准公式[{0}]", formula_std);

		//			switch (conn->DatabaseKind)
		//			{
		//			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//			case DB_KIND_MSSQL:				// MS SQL Server数据库
		//			case DB_KIND_ORACLE:	        // Oracle 数据库
		//			default:						// 所有数据库适用，通用SQL语句
		//				sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
		//					"   FROM TEP0002 "
		//					"  WHERE CODE_CLASS = 'QMYS' "
		//					"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
		//				break;
		//			}
		//			cmd_inq_qmys.SetCommandText(sqlstr);
		//			cmd_inq_qmys.ExecuteReader();
		//			while (cmd_inq_qmys.Read())
		//			{
		//				s_elm_code = cmd_inq_qmys.GetString(1);
		//				s_elm_code_name = cmd_inq_qmys.GetString(2);

		//				Log::Trace("", __FUNCTION__, "*********************************************************************************");
		//				Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

		//				cmd_inq_01.SetCommandText(" SELECT replace(@formula_std,@elm_name,'') FROM DUAL ");
		//				cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//				cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//				cmd_inq_01.ExecuteReader();
		//				Log::Trace("", __FUNCTION__, "替代前formula_std[{0}]", formula_std);
		//				if (cmd_inq_01.Read())
		//				{
		//					formula_std_replace_after = cmd_inq_01.GetString(1);
		//				}
		//				cmd_inq_01.Close();
		//				Log::Trace("", __FUNCTION__, "替代后formula_std_replace_after[{0}]", formula_std_replace_after);


		//				/*若元素在公式中存在，查找是否存在实绩数据*/
		//				if (formula_std_replace_after != formula_std)
		//				{
		//					switch (conn->DatabaseKind)
		//					{
		//					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//					case DB_KIND_MSSQL:				// MS SQL Server数据库
		//					case DB_KIND_ORACLE:	        // Oracle 数据库
		//					default:						// 所有数据库适用，通用SQL语句
		//						sqlstr = " SELECT ELM_NAME ";
		//						if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
		//						if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//						if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//						sqlstr = sqlstr + "   FROM " + s_table_name +
		//							"  WHERE ELM_CODE = @elm_code ";
		//						if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
		//							" AND ST_SAMPLE_NO = @st_sample_no ";
		//						if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
		//						if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
		//						break;
		//					}
		//					cmd_inq_elm.SetCommandText(sqlstr);
		//					cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
		//					cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
		//					cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
		//					cmd_inq_elm.ExecuteReader();
		//					if (cmd_inq_elm.Read())
		//					{
		//						s_elm_name = cmd_inq_elm.GetString(1);

		//						Log::Trace("", __FUNCTION__, "formula_std[{0}],s_elm_name[{1}],elm_value[{2}]", formula_std, s_elm_name, cmd_inq_elm.GetDecimal(2));

		//						cmd_inq_01.SetCommandText(" SELECT replace(@formula_std,@elm_name,@elm_value) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//						cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
		//						cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_std = cmd_inq_01.GetString(1);
		//							Log::Trace("", __FUNCTION__, "formula_std[{0}]", formula_std);
		//						}
		//						cmd_inq_01.Close();
		//					}
		//					else
		//					{
		//						elm_null = elm_null + "," + s_elm_code_name.Trim();
		//						Log::Trace("", __FUNCTION__, "3-未找到实绩的元素：elm_null[{0}]", elm_null);

		//						Log::Trace("", __FUNCTION__, "formula_std[{0}],s_elm_name[{1}]", formula_std, s_elm_code_name);

		//						cmd_inq_01.SetCommandText(" SELECT replace(@formula_std,@elm_name,@elm_value) FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//						cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//						cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
		//						cmd_inq_01.ExecuteReader();
		//						if (cmd_inq_01.Read())
		//						{
		//							formula_std = cmd_inq_01.GetString(1);
		//							Log::Trace("", __FUNCTION__, "formula_std[{0}]", formula_std);
		//						}
		//						cmd_inq_01.Close();
		//					}
		//					cmd_inq_elm.Close();
		//				}
		//			}
		//			cmd_inq_qmys.Close();
		//			Log::Trace("", __FUNCTION__, "【CEQ】标准公式(代入值后)[{0}]", (const char*)formula_std);

		//			if (elm_null.TrimOrBlank() != " ")
		//			{
		//				//if (elm_null.SubstringNE(0, 1) == ",")
		//				//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
		//				//CFormattable arguments[] = { elm_null.Trim() };
		//				//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
		//				//throw CApplicationException(-1, s.msg, log.Location);
		//			}

		//			try
		//			{
		//				cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_std + +",2) FROM DUAL ");
		//				cmd_inq_01.ExecuteReader();
		//				if (cmd_inq_01.Read())
		//				{
		//					s_elm_value = cmd_inq_01.GetDecimal(1);
		//				}
		//				cmd_inq_01.Close();
		//			}
		//			catch (const CException& ex)
		//			{
		//				cmd_inq_01.Close();
		//				strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
		//				throw CApplicationException(-1, s.msg, log.Location);
		//			}
		//			Log::Trace("", __FUNCTION__, "【CEQ】标准公式计算出的值[{0}]", s_elm_value);

		//			CDataRow& row = bcls_ret->Tables[0].Rows.Add();
		//			row["ELM_CODE"] = "C01";
		//			//row["MAIN_MIN"] = 0;
		//			//row["MAIN_MAX"] = 9999.999999;
		//			if (symbol == "<")
		//			{
		//				bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = s_elm_value;
		//				Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的最大值[{0}]", s_elm_value);
		//			}
		//			else if (symbol == ">")
		//			{
		//				bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = s_elm_value;
		//				Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的最小值[{0}]", s_elm_value);
		//			}
		//		}
		//	}
		//	cmd_inq.Close();
		//}
		//Log::Trace("", __FUNCTION__, "pcm_code_Count总数[{0}]", pcm_code_Count);

		//for (int kk = 0; kk < pcm_code_Count; kk++)  /////循环处理PCM
		//{
		//	if (kk == 1)
		//	{
		//		tqmts0x["ELM_FMLA_CODE2"] = tqmts0x["ELM_FMLA_CODE3"];
		//	}
		//	if (kk == 2)
		//	{
		//		tqmts0x["ELM_FMLA_CODE2"] = tqmts0x["ELM_FMLA_CODE4"];
		//	}
		//	if (kk == 3)
		//	{
		//		tqmts0x["ELM_FMLA_CODE2"] = tqmts0x["ELM_FMLA_CODE5"];
		//	}
		//	Log::Trace("", __FUNCTION__, "第[{0}]个PCM组合元素tqmts0x.ELM_FMLA_CODE2 [{1}]",kk, tqmts0x["ELM_FMLA_CODE2"].ToString());

		//	//【PCM】公式计算
		//	if (tqmts0x["ELM_FMLA_CODE2"].ToString().TrimOrBlank() != " " && tqmts0x["ELM_FMLA_CODE2"].ToString().TrimOrBlank() != "00")
		//	{
		//		//获得组合元素【PCM】公式代码
		//		switch (conn->DatabaseKind)
		//		{
		//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//		case DB_KIND_MSSQL:				// MS SQL Server数据库
		//		case DB_KIND_ORACLE:	        // Oracle 数据库
		//		default:						// 所有数据库适用，通用SQL语句
		//			sqlstr = " SELECT CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT "
		//				"   FROM TEP0002 "
		//				"  WHERE CODE_CLASS = 'QM63' "
		//				"    AND CODE = @code ";
		//			break;
		//		}
		//		cmd_inq.SetCommandText(sqlstr);
		//		cmd_inq.Parameters.Set("code", tqmts0x["ELM_FMLA_CODE2"].ToString().Trim());
		//		cmd_inq.ExecuteReader();
		//		if (cmd_inq.Read())
		//		{
		//			formula_value = cmd_inq.GetString(1);
		//			symbol = cmd_inq.GetString(2);
		//			formula_std = cmd_inq.GetString(3);

		//			Log::Trace("", __FUNCTION__, "符号[{0}], 实绩公式[{1}], 标准公式[{1}]", symbol, formula_value, formula_std);

		//			if (formula_value.TrimOrBlank() != " ")
		//			{
		//				Log::Trace("", __FUNCTION__, "实绩公式formula_value 计算开始");
		//				Log::Trace("", __FUNCTION__, "转换绝对值符号");

		//				double j = 0;

		//				for (i = 1; i <= formula_value.GetLength(); i++)
		//				{
		//					if ("|" == formula_value.SubstringNE(i - 1, 1))
		//					{
		//						j = j + 1;

		//						Log::Trace("", __FUNCTION__, "实绩公式第i[{0}]位, 第j[{1}]个'|'", i, j);

		//						if (0 == fmod(j, 2))
		//						{
		//							Log::Trace("", __FUNCTION__, "双数");

		//							cmd_inq_01.SetCommandText(" SELECT substr(@formula_value,1,@i-1) || ')' || substr(@formula_value,@i+1) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//							cmd_inq_01.Parameters.Set("i", i);
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_value = cmd_inq_01.GetString(1);
		//							}
		//							cmd_inq_01.Close();

		//							Log::Trace("", __FUNCTION__, "双数| 实绩公式[{0}]", formula_value);
		//						}
		//						else
		//						{
		//							Log::Trace("", __FUNCTION__, "单数");

		//							cmd_inq_01.SetCommandText(" SELECT substr(@formula_value,1,@i-1) || 'abs(' || substr(@formula_value,@i+1) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//							cmd_inq_01.Parameters.Set("i", i);
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_value = cmd_inq_01.GetString(1);
		//							}
		//							cmd_inq_01.Close();

		//							Log::Trace("", __FUNCTION__, "单数| 实绩公式[{0}]", formula_value);
		//						}
		//					}
		//				}

		//				Log::Trace("", __FUNCTION__, "转换绝对值符号后，实绩公式[{0}]", formula_value);

		//				switch (conn->DatabaseKind)
		//				{
		//				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//				case DB_KIND_MSSQL:				// MS SQL Server数据库
		//				case DB_KIND_ORACLE:	        // Oracle 数据库
		//				default:						// 所有数据库适用，通用SQL语句
		//					sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
		//						"   FROM TEP0002 "
		//						"  WHERE CODE_CLASS = 'QMYS' "
		//						"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
		//					break;
		//				}
		//				cmd_inq_qmys.SetCommandText(sqlstr);
		//				cmd_inq_qmys.ExecuteReader();
		//				while (cmd_inq_qmys.Read())
		//				{
		//					s_elm_code = cmd_inq_qmys.GetString(1);
		//					s_elm_code_name = cmd_inq_qmys.GetString(2);

		//					//Log::Trace("", __FUNCTION__, "*********************************************************************************");
		//					Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

		//					cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,'') FROM DUAL ");
		//					cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//					cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//					cmd_inq_01.ExecuteReader();
		//					Log::Trace("", __FUNCTION__, "替代前formula_value[{0}]", formula_value);
		//					if (cmd_inq_01.Read())
		//					{
		//						formula_value_replace_after = cmd_inq_01.GetString(1);
		//					}
		//					cmd_inq_01.Close();
		//					Log::Trace("", __FUNCTION__, "替代后formula_value_replace_after[{0}]", formula_value_replace_after);


		//					/*若元素在公式中存在，查找是否存在实绩数据*/
		//					if (formula_value_replace_after != formula_value)
		//					{
		//						switch (conn->DatabaseKind)
		//						{
		//						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//						case DB_KIND_MSSQL:				// MS SQL Server数据库
		//						case DB_KIND_ORACLE:	        // Oracle 数据库
		//						default:						// 所有数据库适用，通用SQL语句
		//							sqlstr = " SELECT ELM_NAME ";
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//							sqlstr = sqlstr + "   FROM " + s_table_name +
		//								"  WHERE ELM_CODE = @elm_code ";
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
		//								" AND ST_SAMPLE_NO = @st_sample_no ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
		//							break;
		//						}
		//						cmd_inq_elm.SetCommandText(sqlstr);
		//						cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
		//						cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
		//						cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
		//						cmd_inq_elm.ExecuteReader();
		//						if (cmd_inq_elm.Read())
		//						{
		//							s_elm_name = cmd_inq_elm.GetString(1);

		//							Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_name[{1}],elm_value[{2}]", formula_value, s_elm_name, cmd_inq_elm.GetDecimal(2));

		//							cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//							cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
		//							cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_value = cmd_inq_01.GetString(1);
		//								Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
		//							}
		//							cmd_inq_01.Close();
		//						}
		//						else
		//						{
		//							elm_null = elm_null + "," + s_elm_code_name.Trim();
		//							Log::Trace("", __FUNCTION__, "4-未找到实绩的元素：elm_null[{0}]", elm_null);

		//							Log::Trace("", __FUNCTION__, "formula_value[{0}],s_elm_name[{1}]", formula_value, s_elm_code_name);

		//							cmd_inq_01.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_value) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_value", formula_value);
		//							cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//							cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_value = cmd_inq_01.GetString(1);
		//								Log::Trace("", __FUNCTION__, "formula_value[{0}]", formula_value);
		//							}
		//							cmd_inq_01.Close();
		//						}
		//						cmd_inq_elm.Close();
		//					}
		//				}
		//				cmd_inq_qmys.Close();
		//				Log::Trace("", __FUNCTION__, "【PCM】实绩公式(代入值后)[{0}]", (const char*)formula_value);

		//				if (elm_null.TrimOrBlank() != " ")
		//				{
		//					//if (elm_null.SubstringNE(0, 1) == ",")
		//					//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
		//					//CFormattable arguments[] = { elm_null.Trim() };
		//					//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
		//					//throw CApplicationException(-1, s.msg, log.Location);
		//				}

		//				try
		//				{
		//					cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_value + +",2) FROM DUAL ");
		//					cmd_inq_01.ExecuteReader();
		//					if (cmd_inq_01.Read())
		//					{
		//						s_elm_value = cmd_inq_01.GetDecimal(1);
		//					}
		//					cmd_inq_01.Close();
		//				}
		//				catch (const CException& ex)
		//				{
		//					cmd_inq_01.Close();
		//					strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
		//					throw CApplicationException(-1, s.msg, log.Location);
		//				}
		//				Log::Trace("", __FUNCTION__, "【PCM】实绩公式计算出的值[{0}]", s_elm_value);

		//				if ("Z" == tqmts0x["ELM_FMLA_CODE2"].ToString().SubstringNE(0, 1).ToUpper())
		//				{
		//					Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);

		//					cmd_inq_01.SetCommandText(" SELECT replace(@symbol,'>','-') FROM DUAL ");
		//					cmd_inq_01.Parameters.Set("symbol", symbol);
		//					cmd_inq_01.ExecuteReader();
		//					if (cmd_inq_01.Read())
		//					{
		//						symbol = cmd_inq_01.GetString(1);
		//						Log::Trace("", __FUNCTION__, "> symbol[{0}]", symbol);
		//					}
		//					cmd_inq_01.Close();

		//					cmd_inq_01.SetCommandText(" SELECT replace(@symbol,'<','+') FROM DUAL ");
		//					cmd_inq_01.Parameters.Set("symbol", symbol);
		//					cmd_inq_01.ExecuteReader();
		//					if (cmd_inq_01.Read())
		//					{
		//						symbol = cmd_inq_01.GetString(1);
		//						Log::Trace("", __FUNCTION__, "< symbol[{0}]", symbol);
		//					}
		//					cmd_inq_01.Close();

		//					if (symbol.Find("+") > 0)
		//					{
		//						symbol = "-" + symbol;
		//					}

		//					Log::Trace("", __FUNCTION__, "替换后 symbol[{0}]", symbol);

		//					switch (conn->DatabaseKind)
		//					{
		//					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//					case DB_KIND_MSSQL:				// MS SQL Server数据库
		//					case DB_KIND_ORACLE:	        // Oracle 数据库
		//					default:						// 所有数据库适用，通用SQL语句
		//						sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
		//							"   FROM TEP0002 "
		//							"  WHERE CODE_CLASS = 'QMYS' "
		//							"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
		//						break;
		//					}
		//					cmd_inq_qmys.SetCommandText(sqlstr);
		//					cmd_inq_qmys.ExecuteReader();
		//					while (cmd_inq_qmys.Read())
		//					{
		//						s_elm_code = cmd_inq_qmys.GetString(1);
		//						s_elm_code_name = cmd_inq_qmys.GetString(2);

		//						//Log::Trace("", __FUNCTION__, "*********************************************************************************");
		//						//Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

		//						cmd_inq_01.SetCommandText(" SELECT replace(@symbol,@elm_name,'') FROM DUAL ");
		//						cmd_inq_01.Parameters.Set("symbol", symbol);
		//						cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//						cmd_inq_01.ExecuteReader();
		//						//Log::Trace("", __FUNCTION__, "替代前symbol[{0}]", symbol);
		//						if (cmd_inq_01.Read())
		//						{
		//							symbol_replace_after = cmd_inq_01.GetString(1);
		//						}
		//						cmd_inq_01.Close();
		//						//Log::Trace("", __FUNCTION__, "替代后symbol_replace_after[{0}]", symbol_replace_after);


		//						/*若元素在公式中存在，查找是否存在实绩数据*/
		//						if (symbol_replace_after != symbol)
		//						{
		//							switch (conn->DatabaseKind)
		//							{
		//							case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//							case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//							case DB_KIND_MSSQL:				// MS SQL Server数据库
		//							case DB_KIND_ORACLE:	        // Oracle 数据库
		//							default:						// 所有数据库适用，通用SQL语句
		//								sqlstr = " SELECT ELM_NAME ";
		//								if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
		//								if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//								if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//								sqlstr = sqlstr + "   FROM " + s_table_name +
		//									"  WHERE ELM_CODE = @elm_code ";
		//								if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
		//									" AND ST_SAMPLE_NO = @st_sample_no ";
		//								if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
		//								if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
		//								break;
		//							}
		//							cmd_inq_elm.SetCommandText(sqlstr);
		//							cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
		//							cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
		//							cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
		//							cmd_inq_elm.ExecuteReader();
		//							if (cmd_inq_elm.Read())
		//							{
		//								s_elm_name = cmd_inq_elm.GetString(1);

		//								Log::Trace("", __FUNCTION__, "symbol[{0}],s_elm_name[{1}],elm_value[{2}]", symbol, s_elm_name, cmd_inq_elm.GetDecimal(2));

		//								cmd_inq_01.SetCommandText(" SELECT replace(@symbol,@elm_name,@elm_value) FROM DUAL ");
		//								cmd_inq_01.Parameters.Set("symbol", symbol);
		//								cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
		//								cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
		//								cmd_inq_01.ExecuteReader();
		//								if (cmd_inq_01.Read())
		//								{
		//									symbol = cmd_inq_01.GetString(1);
		//									Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
		//								}
		//								cmd_inq_01.Close();
		//							}
		//							else
		//							{
		//								elm_null = elm_null + "," + s_elm_code_name.Trim();
		//								Log::Trace("", __FUNCTION__, "5-未找到实绩的元素：elm_null[{0}]", elm_null);

		//								Log::Trace("", __FUNCTION__, "symbol[{0}],s_elm_name[{1}]", symbol, s_elm_code_name);

		//								cmd_inq_01.SetCommandText(" SELECT replace(@symbol,@elm_name,@elm_value) FROM DUAL ");
		//								cmd_inq_01.Parameters.Set("symbol", symbol);
		//								cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//								cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
		//								cmd_inq_01.ExecuteReader();
		//								if (cmd_inq_01.Read())
		//								{
		//									symbol = cmd_inq_01.GetString(1);
		//									Log::Trace("", __FUNCTION__, "symbol[{0}]", symbol);
		//								}
		//								cmd_inq_01.Close();
		//							}
		//							cmd_inq_elm.Close();
		//						}
		//					}
		//					cmd_inq_qmys.Close();

		//					Log::Trace("", __FUNCTION__, "symbol公式(代入值后)[{0}]", (const char*)symbol);

		//					if (elm_null.TrimOrBlank() != " ")
		//					{
		//						//if (elm_null.SubstringNE(0, 1) == ",")
		//						//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
		//						//CFormattable arguments[] = { elm_null.Trim() };
		//						//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
		//						//throw CApplicationException(-1, s.msg, log.Location);
		//					}

		//					cmd_inq_01.SetCommandText(" SELECT " + symbol + " FROM DUAL ");
		//					cmd_inq_01.ExecuteReader();
		//					if (cmd_inq_01.Read())
		//					{
		//						symbol_val = cmd_inq_01.GetDecimal(1);
		//					}
		//					cmd_inq_01.Close();

		//					Log::Trace("", __FUNCTION__, "条件计算结构 symbol_val[{0}]", symbol_val);


		//					if (symbol_val >= 0)
		//					{
		//						//新增 组合元素实绩值
		//						switch (conn->DatabaseKind)
		//						{
		//						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//						case DB_KIND_MSSQL:				// MS SQL Server数据库
		//						case DB_KIND_ORACLE:	        // Oracle 数据库
		//						default:						// 所有数据库适用，通用SQL语句
		//							sqlstr = " INSERT INTO " + s_table_name;
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//								" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@elm_code,@elm_name,@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//								" VALUES(@pono,@heat_no,@st_no,'C03','PCM',@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " (PONO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//								" VALUES(@pono,@st_sample_no,@st_no,'C03','PCM',@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//							break;
		//						}
		//						cmd_ins.SetCommandText(sqlstr);
		//						cmd_ins.Parameters.Set("pono", s_pono);
		//						cmd_ins.Parameters.Set("heat_no", s_heat_no);
		//						cmd_ins.Parameters.Set("st_sample_no", s_st_sample_no);
		//						cmd_ins.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
		//						cmd_ins.Parameters.Set("elm_value", s_elm_value);
		//						cmd_ins.Parameters.Set("rec_creator", "DBCOUNT");
		//						cmd_ins.Parameters.Set("rec_create_time", s_rec_create_time);
		//						cmd_ins.Parameters.Set("elm_code", tqmts0x["ELM_FMLA_CODE2"].ToString());
		//						cmd_ins.Parameters.Set("elm_name", tqmts0x["ELM_FMLA_CODE2"].ToString());
		//						cmd_ins.Parameters.Set("whole_backlog_code", "C");
		//						cmd_ins.ExecuteNonQuery();
		//						cmd_ins.Close();
		//					}
		//				}
		//				else
		//				{
		//					//新增 组合元素实绩值
		//					switch (conn->DatabaseKind)
		//					{
		//					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//					case DB_KIND_MSSQL:				// MS SQL Server数据库
		//					case DB_KIND_ORACLE:	        // Oracle 数据库
		//					default:						// 所有数据库适用，通用SQL语句
		//						sqlstr = " INSERT INTO " + s_table_name;
		//						if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//							" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@elm_code,@elm_name,@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//						if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " (PONO,HEAT_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//							" VALUES(@pono,@heat_no,@st_no,'C03','PCM',@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//						if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " (PONO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_VALUE,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
		//							" VALUES(@pono,@st_sample_no,@st_no,'C03','PCM',@elm_value,'0', @rec_creator, @rec_create_time,@whole_backlog_code) ";
		//						break;
		//					}
		//					cmd_ins.SetCommandText(sqlstr);
		//					cmd_ins.Parameters.Set("pono", s_pono);
		//					cmd_ins.Parameters.Set("heat_no", s_heat_no);
		//					cmd_ins.Parameters.Set("st_sample_no", s_st_sample_no);
		//					cmd_ins.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
		//					cmd_ins.Parameters.Set("elm_value", s_elm_value);
		//					cmd_ins.Parameters.Set("rec_creator", s_elm_value);
		//					cmd_ins.Parameters.Set("rec_create_time", s_elm_value);
		//					cmd_ins.Parameters.Set("rec_creator", "DBCOUNT");
		//					cmd_ins.Parameters.Set("rec_create_time", s_rec_create_time);
		//					cmd_ins.Parameters.Set("elm_code", tqmts0x["ELM_FMLA_CODE2"].ToString());
		//					cmd_ins.Parameters.Set("elm_name", tqmts0x["ELM_FMLA_CODE2"].ToString());
		//					cmd_ins.Parameters.Set("whole_backlog_code", "C");
		//					cmd_ins.ExecuteNonQuery();
		//					cmd_ins.Close();
		//				}
		//			}

		//			if (formula_std.TrimOrBlank() != " ")
		//			{
		//				Log::Trace("", __FUNCTION__, "标准公式formula_std 计算开始");
		//				Log::Trace("", __FUNCTION__, "转换绝对值符号");

		//				double j = 0;

		//				for (i = 1; i <= formula_std.GetLength(); i++)
		//				{
		//					if ("|" == formula_std.SubstringNE(i - 1, 1))
		//					{
		//						j = j + 1;

		//						Log::Trace("", __FUNCTION__, "标准公式第i[{0}]位, 第j[{1}]个'|'", i, j);

		//						if (0 == fmod(j, 2))
		//						{
		//							Log::Trace("", __FUNCTION__, "双数");

		//							cmd_inq_01.SetCommandText(" SELECT substr(@formula_std,1,@i-1) || ')' || substr(@formula_std,@i+1) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//							cmd_inq_01.Parameters.Set("i", i);
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_std = cmd_inq_01.GetString(1);
		//							}
		//							cmd_inq_01.Close();

		//							Log::Trace("", __FUNCTION__, "双数| 标准公式[{0}]", formula_std);
		//						}
		//						else
		//						{
		//							Log::Trace("", __FUNCTION__, "单数");

		//							cmd_inq_01.SetCommandText(" SELECT substr(@formula_std,1,@i-1) || 'abs(' || substr(@formula_std,@i+1) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//							cmd_inq_01.Parameters.Set("i", i);
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_std = cmd_inq_01.GetString(1);
		//							}
		//							cmd_inq_01.Close();

		//							Log::Trace("", __FUNCTION__, "单数| 标准公式[{0}]", formula_std);
		//						}
		//					}
		//				}

		//				Log::Trace("", __FUNCTION__, "转换绝对值符号后，标准公式[{0}]", formula_std);

		//				switch (conn->DatabaseKind)
		//				{
		//				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//				case DB_KIND_MSSQL:				// MS SQL Server数据库
		//				case DB_KIND_ORACLE:	        // Oracle 数据库
		//				default:						// 所有数据库适用，通用SQL语句
		//					sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT "
		//						"   FROM TEP0002 "
		//						"  WHERE CODE_CLASS = 'QMYS' "
		//						"  ORDER BY lengthb(CODE_DESC_1_CONTENT) desc ";/*根据元素英文名长度倒排序*/
		//					break;
		//				}
		//				cmd_inq_qmys.SetCommandText(sqlstr);
		//				cmd_inq_qmys.ExecuteReader();
		//				while (cmd_inq_qmys.Read())
		//				{
		//					s_elm_code = cmd_inq_qmys.GetString(1);
		//					s_elm_code_name = cmd_inq_qmys.GetString(2);

		//					//Log::Trace("", __FUNCTION__, "*********************************************************************************");
		//					//Log::Trace("", __FUNCTION__, "s_elm_code[{0}],s_elm_code_name[{1}]", s_elm_code, s_elm_code_name);

		//					cmd_inq_01.SetCommandText(" SELECT replace(@formula_std,@elm_name,'') FROM DUAL ");
		//					cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//					cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//					cmd_inq_01.ExecuteReader();
		//					//Log::Trace("", __FUNCTION__, "替代前formula_std[{0}]", formula_std);
		//					if (cmd_inq_01.Read())
		//					{
		//						formula_std_replace_after = cmd_inq_01.GetString(1);
		//					}
		//					cmd_inq_01.Close();
		//					//Log::Trace("", __FUNCTION__, "替代后formula_std_replace_after[{0}]", formula_std_replace_after);


		//					/*若元素在公式中存在，查找是否存在实绩数据*/
		//					if (formula_std_replace_after != formula_std)
		//					{
		//						switch (conn->DatabaseKind)
		//						{
		//						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//						case DB_KIND_MSSQL:				// MS SQL Server数据库
		//						case DB_KIND_ORACLE:	        // Oracle 数据库
		//						default:						// 所有数据库适用，通用SQL语句
		//							sqlstr = " SELECT ELM_NAME ";
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + ", ELM_ACT ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + ", ELM_VALUE ";
		//							sqlstr = sqlstr + "   FROM " + s_table_name +
		//								"  WHERE ELM_CODE = @elm_code ";
		//							if ("TQMTS25" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no "
		//								" AND ST_SAMPLE_NO = @st_sample_no ";
		//							if ("TQMTS29" == s_table_name)	sqlstr = sqlstr + " AND HEAT_NO = @heat_no ";
		//							if ("TQMTS2C" == s_table_name)	sqlstr = sqlstr + " AND ST_SAMPLE_NO = @st_sample_no ";
		//							break;
		//						}
		//						cmd_inq_elm.SetCommandText(sqlstr);
		//						cmd_inq_elm.Parameters.Set("heat_no", s_heat_no);
		//						cmd_inq_elm.Parameters.Set("st_sample_no", s_st_sample_no);
		//						cmd_inq_elm.Parameters.Set("elm_code", s_elm_code);
		//						cmd_inq_elm.ExecuteReader();
		//						if (cmd_inq_elm.Read())
		//						{
		//							s_elm_name = cmd_inq_elm.GetString(1);

		//							Log::Trace("", __FUNCTION__, "formula_std[{0}],s_elm_name[{1}],elm_value[{2}]", formula_std, s_elm_name, cmd_inq_elm.GetDecimal(2));

		//							cmd_inq_01.SetCommandText(" SELECT replace(@formula_std,@elm_name,@elm_value) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//							cmd_inq_01.Parameters.Set("elm_name", s_elm_name);
		//							cmd_inq_01.Parameters.Set("elm_value", cmd_inq_elm.GetDecimal(2));
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_std = cmd_inq_01.GetString(1);
		//								Log::Trace("", __FUNCTION__, "formula_std[{0}]", formula_std);
		//							}
		//							cmd_inq_01.Close();
		//						}
		//						else
		//						{
		//							elm_null = elm_null + "," + s_elm_code_name.Trim();
		//							Log::Trace("", __FUNCTION__, "6-未找到实绩的元素：elm_null[{0}]", elm_null);

		//							Log::Trace("", __FUNCTION__, "formula_std[{0}],s_elm_name[{1}]", formula_std, s_elm_code_name);

		//							cmd_inq_01.SetCommandText(" SELECT replace(@formula_std,@elm_name,@elm_value) FROM DUAL ");
		//							cmd_inq_01.Parameters.Set("formula_std", formula_std);
		//							cmd_inq_01.Parameters.Set("elm_name", s_elm_code_name);
		//							cmd_inq_01.Parameters.Set("elm_value", 0); ////找不到实绩，赋值0
		//							cmd_inq_01.ExecuteReader();
		//							if (cmd_inq_01.Read())
		//							{
		//								formula_std = cmd_inq_01.GetString(1);
		//								Log::Trace("", __FUNCTION__, "formula_std[{0}]", formula_std);
		//							}
		//							cmd_inq_01.Close();
		//						}
		//						cmd_inq_elm.Close();
		//					}
		//				}
		//				cmd_inq_qmys.Close();
		//				Log::Trace("", __FUNCTION__, "【PCM】标准公式(代入值后)[{0}]", (const char*)formula_std);

		//				if (elm_null.TrimOrBlank() != " ")
		//				{
		//					//if (elm_null.SubstringNE(0, 1) == ",")
		//					//	elm_null = elm_null.SubstringNE(1, elm_null.GetLength() - 1);
		//					//CFormattable arguments[] = { elm_null.Trim() };
		//					//CMessageFormat::Format(s.msg, _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
		//					//throw CApplicationException(-1, s.msg, log.Location);
		//				}

		//				try
		//				{
		//					cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_std + +",2) FROM DUAL ");
		//					cmd_inq_01.ExecuteReader();
		//					if (cmd_inq_01.Read())
		//					{
		//						s_elm_value = cmd_inq_01.GetDecimal(1);
		//					}
		//					cmd_inq_01.Close();
		//				}
		//				catch (const CException& ex)
		//				{
		//					cmd_inq_01.Close();
		//					strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
		//					throw CApplicationException(-1, s.msg, log.Location);
		//				}
		//				Log::Trace("", __FUNCTION__, "【PCM】标准公式计算出的值[{0}]", s_elm_value);

		//				CDataRow& row = bcls_ret->Tables[0].Rows.Add();
		//				row["ELM_CODE"] = "C03";
		//				//row["MAIN_MIN"] = 0;
		//				//row["MAIN_MAX"] = 9999.999999;
		//				if (symbol == "<")
		//				{
		//					bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = s_elm_value;
		//					Log::Trace("", __FUNCTION__, "【PCM】公式计算出的最大值[{0}]", s_elm_value);
		//				}
		//				else if (symbol == ">")
		//				{
		//					bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = s_elm_value;
		//					Log::Trace("", __FUNCTION__, "【PCM】公式计算出的最小值[{0}]", s_elm_value);
		//				}
		//			}
		//		}
		//		cmd_inq.Close();
		//	}

		//}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		Log::Trace("", "", "1111111111111111");
		//CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Trace("", "", "%s", (const char*)ex.GetMsg());
		sprintf(s.msg, "erorr[%s]", (const char*)ex.GetMsg());
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		Log::Trace("", "", "s.msg = [%s]", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		Log::Trace("", "", "2222222222222222");
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		Log::Trace("", "", "3333333333333333");
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		strncpy(s.sysmsg, "System Exception", sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
