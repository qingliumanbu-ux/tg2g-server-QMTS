/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-18
Description: 炼钢试样工序成分判定函数
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
#include "tqmts02.h"
#include "tqmts24.h"
#include "tqmts25.h"
#include "tqmts0x.h"
#include "tqmts09.h"
#include "math.h"

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炼钢试样工序成分判定函数
/// <para>
/// 1.炼钢试样工序成分判定函数。
/// 1.查询试样成分主信息;
/// 2.工序成分标准信息;
/// 3.混杂元素标准:从TQMTS04中查询指定的混杂元素判定标准代码，再从TQMTS09中取出标准;
/// 4.以设定的标准元素循环，逐次来判定试样成分值
///  1)取成分元素实绩值
///  2)没有试样值时，看标准无要求，没有则判成合格；有判成不合格
/// 5.有试样值时:
///  1)看试样类型（st_sample_div），对钢样1，气体样2分别判定;
///  2)成分判定:
///    标准值为0～0，则判混杂元素；混杂元素管理区分在制造标准中：有值，则判；无值，则判成2——主试不合格;
///    工序成分的判定, 除了连铸工序以外, 都只判主试;
///    在主试上下限范围内的，判合;
///    主试不合时，无特采要求的判为2（主试不合格）;
///    有特采要求的，在上下限范围内的，判为3（主试不合但特采合）;
///    不在上下限范围内的，判为1（主试不合且特采不合）;
/// 6.记录判定结果
/// 
/// </para>
/// <para>数据库表：TQMTS24(试样成分主信息)
///												                </para>
/// <para>主调用函数： 
/// <para>需调用函数： 
/// </summary>
/// <param name="st_sample_no">炼钢试样号             </param>
/// <returns>  </returns>
===========================================================</remark>*/

int f_qmts25_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString symbol = "";
	CString formula_value = "";
	CString elm_null = "";
	int formula_len = 0;
	CDecimal cu_ni_cr = 0;
	CDecimal as_sn_nb_v_ti_zr_b = 0;

	int elm_ok = 0;//成分合否标志 0:未判定,1:主试不合且特采不合,2:主试不合且无特采,3:主试不合但特采合,8:主试合,9:不需判
	int elm_0_num = 0;//未收到实绩的元素数量
	int elm_1_num = 0;//判定结果为不合的元素数量

	CString sqlstr = "";

	/* 实体类定义 */
	CTQMTS02 tqmts02(conn);
	CTQMTS24 tqmts24(conn);
	CTQMTS25 tqmts25(conn);
	CTQMTS0X tqmts0x(conn);
	CTQMTS09 tqmts09(conn);

	EIClass bcls_elm;//存放元素标准
	bcls_elm.Tables[0].Columns.Add(tqmts02);
	bcls_elm.Tables[0].Columns.Add(DT_INT16,"ELM_OK");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_02(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		/*获得传入参数*/
		tqmts24.HEAT_NO = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tqmts24.ST_SAMPLE_NO = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim();
		tqmts24.ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		//tqmts24.FACTORY_DIV = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();//厂别区分


		Log::Trace("", __FUNCTION__, "f_qmts25_jud IN:---heat_no[{0}]",(const char*)tqmts24.HEAT_NO);
		Log::Trace("", __FUNCTION__, "f_qmts25_jud IN:---st_sample_no[{0}]",(const char*)tqmts24.ST_SAMPLE_NO);
		Log::Trace("", __FUNCTION__, "f_qmts25_jud IN:---st_no[{0}]",(const char*)tqmts24.ST_NO);
		//Log::Trace("", __FUNCTION__, "f_qmts25_jud IN:---factory_div[{0}]",(const char*)tqmts24.FACTORY_DIV);

		//校验传入参数
		if(tqmts24.HEAT_NO.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[熔炼号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts24.ST_SAMPLE_NO.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[炼钢试样号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts24.ST_NO.TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[出钢记号不允许为空]");
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/*------------------------  查询试样成分主信息  ------------------------*/
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV "
				"   FROM TQMTS24 "
				"  WHERE HEAT_NO = @tqmts24.HEAT_NO "
				"    AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("tqmts24.HEAT_NO", tqmts24.HEAT_NO);
		cmd_inq.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24.ST_SAMPLE_NO);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts24.WHOLE_BACKLOG_CODE = cmd_inq.GetString(1);
			tqmts24.ST_SAMPLE_DIV = cmd_inq.GetString(2);
			tqmts24.GAS_TYPE_DIV = cmd_inq.GetString(3);
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, " tqmts24.whole_backlog_code=[{0}] ",(const char*)tqmts24.WHOLE_BACKLOG_CODE);
		Log::Trace("", __FUNCTION__, " tqmts24.st_sample_div=[{0}] ",(const char*)tqmts24.ST_SAMPLE_DIV);
		Log::Trace("", __FUNCTION__, " tqmts24.gas_type_div=[{0}] ",(const char*)tqmts24.GAS_TYPE_DIV);

		//脱磷采用转炉成分标准 add by FXY 2012-10-15
		if (tqmts24.WHOLE_BACKLOG_CODE.TrimOrBlank() == "P")
		{
			tqmts24.WHOLE_BACKLOG_CODE = "B";
		}

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT NVL(TRIM(O_GAS_DIV),'0'),NVL(TRIM(N_GAS_DIV),'0'),NVL(TRIM(H_GAS_DIV),'0') "
				"  FROM TQMTS0X "
				" WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("st_no", tqmts24.ST_NO);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts0x.O_GAS_DIV = cmd_inq.GetString(1);
			tqmts0x.N_GAS_DIV = cmd_inq.GetString(2);
			tqmts0x.H_GAS_DIV = cmd_inq.GetString(3);
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "气体试样区分 O[{0}]N[{1}]H[{2}]",(const char*)tqmts0x.O_GAS_DIV,(const char*)tqmts0x.N_GAS_DIV,(const char*)tqmts0x.H_GAS_DIV);

		/*------------------------  查询工序成分标准信息  ------------------------*/
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS02 "
				" WHERE ST_NO = @st_no "
				"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
				"   AND SMELT_CHEMI_FLAG >= '3'";/*3-当前元素要判定*/
			break;
			//"   AND FACTORY_DIV        = @tqmts24.FACTORY_DIV ";
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("st_no", tqmts24.ST_NO);
		cmd_inq.Parameters.Set("whole_backlog_code", tqmts24.WHOLE_BACKLOG_CODE);
		Log::Trace("", __FUNCTION__, "ST_NO=[{0}]",(const char*)tqmts24.ST_NO);
		Log::Trace("", __FUNCTION__, "WHOLE_BACKLOG_CODE=[{0}]",(const char*)tqmts24.WHOLE_BACKLOG_CODE);
		//cmd_inq.Parameters.Set("tqmts24.FACTORY_DIV", tqmts24.FACTORY_DIV);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;
			cmd_inq.Fetch(tqmts02);
			CDataRow& row = bcls_elm.Tables[0].Rows.Add();
			row.Merge(tqmts02);
			row["ELM_OK"] = elm_ok;
		}
		cmd_inq.Close();
		if(fetchRowCount <= 0)
		{
			sprintf(s.msg,"出钢记号[%s]在工序[%s]中的成分标准不存在，请先维护成分标准",(const char*)tqmts24.ST_NO,(const char*)tqmts24.WHOLE_BACKLOG_CODE);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/*------------------------  计算组合元素成分标准  ------------------------*/
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT ELM_FMLA_CODE1,ELM_FMLA_CODE2,SMELT_DIV "  /*SMELT_DIV:  B-转炉；E-电炉*/
				"  FROM TQMTS0X "
				" WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", tqmts24.ST_NO.Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts0x.ELM_FMLA_CODE1 = cmd_inq.GetString(1);
			tqmts0x.ELM_FMLA_CODE2 = cmd_inq.GetString(2);
			tqmts0x.SMELT_DIV = cmd_inq.GetString(3);
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "【CEQ】公式代码[{0}]，【PCM】公式代码[{1}]",(const char*)tqmts0x.ELM_FMLA_CODE1,(const char*)tqmts0x.ELM_FMLA_CODE2);
		if (tqmts0x.ELM_FMLA_CODE1.TrimOrBlank() != " " && tqmts0x.ELM_FMLA_CODE1.TrimOrBlank() != "00")
		{
			//获得组合元素【CEQ】公式代码
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QM63' "
					"   AND CODE = @code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", tqmts0x.ELM_FMLA_CODE1.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				symbol = cmd_inq.GetString(1);
				formula_value = cmd_inq.GetString(2);
				Log::Trace("", __FUNCTION__, "【CEQ】符号[{0}]公式[{1}]",(const char*)symbol,(const char*)formula_value);

				if(formula_value.TrimOrBlank() != " ")
				{
					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT * "
							"  FROM TQMTS25 "
							" WHERE HEAT_NO = @heat_no "
							"   AND ST_SAMPLE_NO = @st_sample_no"
							" ORDER BY lengthb(ELM_NAME) desc ";
						break;
					}
					cmd_inq_01.SetCommandText(sqlstr);
					cmd_inq_01.Parameters.Set("heat_no", tqmts24.HEAT_NO);
					cmd_inq_01.Parameters.Set("st_sample_no", tqmts24.ST_SAMPLE_NO);
					cmd_inq_01.ExecuteReader();
					while (cmd_inq_01.Read())
					{
						cmd_inq_01.Fetch(tqmts25);
						cmd_inq_02.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_act) FROM DUAL ");
						cmd_inq_02.Parameters.Set("formula_value", formula_value);
						cmd_inq_02.Parameters.Set("elm_name", tqmts25.ELM_NAME);
						cmd_inq_02.Parameters.Set("elm_act", tqmts25.ELM_ACT);
						cmd_inq_02.ExecuteReader();
						if(cmd_inq_02.Read())
						{
							formula_value = cmd_inq_02.GetString(1);
						}
						cmd_inq_02.Close();
					}
					cmd_inq_01.Close();
					Log::Trace("", __FUNCTION__, "【CEQ】公式(代入值后)[{0}]",(const char*)formula_value);

					elm_null = formula_value;
					formula_len = formula_value.GetLength();
					Log::Trace("", __FUNCTION__, "formula_len[{0}]",formula_len);
					//20130427 HYF SubstringNE
					for(i=0; i<formula_len;)
					{
						if((elm_null.SubstringNE(i,1)<="9" && elm_null.SubstringNE(i,1)>="0")
							||elm_null.SubstringNE(i,1)=="."
							||elm_null.SubstringNE(i,1)=="("
							||elm_null.SubstringNE(i,1)==")")
						{
							if(i>0)
							{
								elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
							}
							else
							{
								elm_null = elm_null.SubstringNE(i+1,formula_len);
							}
							formula_len--;
							Log::Trace("", __FUNCTION__, "0~9 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
						}
						else if(elm_null.SubstringNE(i,1)=="+"
							||elm_null.SubstringNE(i,1)=="-"
							||elm_null.SubstringNE(i,1)=="*"
							||elm_null.SubstringNE(i,1)=="/")
						{
							if(i>0)
							{
								if(elm_null.SubstringNE(i-1,1)==",")
								{
									elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
									formula_len--;
									Log::Trace("", __FUNCTION__, ", elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
								}
								else
								{
									elm_null[i] = ',';
									i++;
									Log::Trace("", __FUNCTION__, "!, elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
								}
							}
							else
							{
								elm_null = elm_null.SubstringNE(i+1,formula_len);
								formula_len--;
								Log::Trace("", __FUNCTION__, "i0 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
							}
						}
						else
						{
							i++;
							Log::Trace("", __FUNCTION__, "elm elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
						}
					}
					Log::Trace("", __FUNCTION__, "elm_null = [{0}]",(const char*)elm_null);
					if(elm_null.TrimOrBlank() != " ")
					{
						if(elm_null.SubstringNE(elm_null.GetLength()-1,1) == ",")
							elm_null = elm_null.SubstringNE(0,elm_null.GetLength()-1);
						CFormattable arguments[] = { elm_null.Trim() };
						CMessageFormat::Format(s.msg,  _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}

					try
					{
						cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_value + + ",2) FROM DUAL ");
						cmd_inq_01.ExecuteReader();
						if (cmd_inq_01.Read())
						{
							for(i=0;i<bcls_elm.Tables[0].Rows.get_Count();i++)
							{
								if(bcls_elm.Tables[0].Rows[i]["ELM_CODE"].ToString() == "C01")
									break;
							}
							Log::Trace("", __FUNCTION__, "CEQ i = [{0}]",i);
							if(symbol == "<")
							{
								tqmts02.MAIN_MAX = cmd_inq_01.GetDecimal(1);
								bcls_elm.Tables[0].Rows[i]["MAIN_MAX"] = tqmts02.MAIN_MAX;
								Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的最大值[{0}]",tqmts02.MAIN_MAX.ToDouble());
							}
							else if(symbol == ">")
							{
								tqmts02.MAIN_MIN = cmd_inq_01.GetDecimal(1);
								bcls_elm.Tables[0].Rows[i]["MAIN_MIN"] = tqmts02.MAIN_MIN;
								Log::Trace("", __FUNCTION__, "【CEQ】公式计算出的最小值[{0}]",tqmts02.MAIN_MIN.ToDouble());
							}
						}
						cmd_inq_01.Close();
					}
					catch(const CException& ex)
					{
						cmd_inq_01.Close();
						strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "ERROR------[组合元素【CEQ】公式代码内容未配置]");
				strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
		}
		if (tqmts0x.ELM_FMLA_CODE2.TrimOrBlank() != " " && tqmts0x.ELM_FMLA_CODE2.TrimOrBlank() != "00")
		{
			//获得组合元素【PCM】公式代码
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QMA1' "
					"   AND CODE = @code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", tqmts0x.ELM_FMLA_CODE2.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				symbol = cmd_inq.GetString(1);
				formula_value = cmd_inq.GetString(2);
				Log::Trace("", __FUNCTION__, "【PCM】符号[{0}]公式[{1}]",(const char*)symbol,(const char*)formula_value);

				if(formula_value.TrimOrBlank() != " ")
				{
					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT * "
							"  FROM TQMTS25 "
							" WHERE HEAT_NO = @heat_no "
							"   AND ST_SAMPLE_NO = @st_sample_no"
							" ORDER BY lengthb(ELM_NAME) desc ";
						break;
					}
					cmd_inq_01.SetCommandText(sqlstr);
					cmd_inq_01.Parameters.Set("heat_no", tqmts24.HEAT_NO);
					cmd_inq_01.Parameters.Set("st_sample_no", tqmts24.ST_SAMPLE_NO);
					cmd_inq_01.ExecuteReader();
					while (cmd_inq_01.Read())
					{
						cmd_inq_01.Fetch(tqmts25);
						cmd_inq_02.SetCommandText(" SELECT replace(@formula_value,@elm_name,@elm_act) FROM DUAL ");
						cmd_inq_02.Parameters.Set("formula_value", formula_value);
						cmd_inq_02.Parameters.Set("elm_name", tqmts25.ELM_NAME);
						cmd_inq_02.Parameters.Set("elm_act", tqmts25.ELM_ACT);
						cmd_inq_02.ExecuteReader();
						if(cmd_inq_02.Read())
						{
							formula_value = cmd_inq_02.GetString(1);
						}
						cmd_inq_02.Close();
					}
					cmd_inq_01.Close();
					Log::Trace("", __FUNCTION__, "【PCM】公式(代入值后)[{0}]",(const char*)formula_value);

					elm_null = formula_value;
					formula_len = formula_value.GetLength();
					Log::Trace("", __FUNCTION__, "formula_len[{0}]",formula_len);
					for(i=0; i<formula_len;)
					{
						if((elm_null.SubstringNE(i,1)<="9" && elm_null.SubstringNE(i,1)>="0")
							||elm_null.SubstringNE(i,1)=="."
							||elm_null.SubstringNE(i,1)=="("
							||elm_null.SubstringNE(i,1)==")")
						{
							if(i>0)
							{
								elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
							}
							else
							{
								elm_null = elm_null.SubstringNE(i+1,formula_len);
							}
							formula_len--;
							Log::Trace("", __FUNCTION__, "0~9 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
						}
						else if(elm_null.SubstringNE(i,1)=="+"
							||elm_null.SubstringNE(i,1)=="-"
							||elm_null.SubstringNE(i,1)=="*"
							||elm_null.SubstringNE(i,1)=="/")
						{
							if(i>0)
							{
								if(elm_null.SubstringNE(i-1,1)==",")
								{
									elm_null = elm_null.SubstringNE(0,i) + elm_null.SubstringNE(i+1,formula_len);
									formula_len--;
									Log::Trace("", __FUNCTION__, ", elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
								}
								else
								{
									elm_null[i] = ',';
									i++;
									Log::Trace("", __FUNCTION__, "!, elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
								}
							}
							else
							{
								elm_null = elm_null.SubstringNE(i+1,formula_len);
								formula_len--;
								Log::Trace("", __FUNCTION__, "i0 elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
							}
						}
						else
						{
							i++;
							Log::Trace("", __FUNCTION__, "elm elm_null[{0}]formula_len[{1}]i[{2}]",(const char*)elm_null,formula_len,i);
						}
					}
					Log::Trace("", __FUNCTION__, "elm_null = [{0}]",(const char*)elm_null);
					if(elm_null.TrimOrBlank() != " ")
					{
						if(elm_null.SubstringNE(elm_null.GetLength()-1,1) == ",")
							elm_null = elm_null.SubstringNE(0,elm_null.GetLength()-1);
						CFormattable arguments[] = { elm_null.Trim() };
						CMessageFormat::Format(s.msg,  _RES("QM00S0005939")/*元素[{0}]缺少实绩，请录入这些元素的实绩！*/, arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}

					try
					{
						cmd_inq_01.SetCommandText(" SELECT ROUND(" + formula_value + + ",2) FROM DUAL ");
						cmd_inq_01.ExecuteReader();
						if (cmd_inq_01.Read())
						{
							for(i=0;i<bcls_elm.Tables[0].Rows.get_Count();i++)
							{
								if(bcls_elm.Tables[0].Rows[i]["ELM_CODE"].ToString() == "C03")
									break;
							}
							Log::Trace("", __FUNCTION__, "PCM i = [{0}]",i);
							if(symbol == "<")
							{
								tqmts02.MAIN_MAX = cmd_inq_01.GetDecimal(1);
								bcls_elm.Tables[0].Rows[i]["MAIN_MAX"] = tqmts02.MAIN_MAX;
								Log::Trace("", __FUNCTION__, "【PCM】公式计算出的最大值[{0}]",tqmts02.MAIN_MAX.ToDouble());
							}
							else if(symbol == ">")
							{
								tqmts02.MAIN_MIN = cmd_inq_01.GetDecimal(1);
								bcls_elm.Tables[0].Rows[i]["MAIN_MIN"] = tqmts02.MAIN_MIN;
								Log::Trace("", __FUNCTION__, "【PCM】公式计算出的最小值[{0}]",tqmts02.MAIN_MIN.ToDouble());
							}
						}
						cmd_inq_01.Close();
					}
					catch(const CException& ex)
					{
						cmd_inq_01.Close();
						strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "ERROR------[组合元素【PCM】公式代码内容未配置]");
				strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
		}

		Log::Trace("", __FUNCTION__, "tqmts24.ST_NO = [{0}]",(const char*)tqmts24.ST_NO);
		/*------------------------  查询混杂元素标准信息  ------------------------*/
		//从TQMTS04中查询指定的混杂元素判定标准代码，再从TQMTS09中取出标准
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS09 "
				" WHERE HZGL IN (SELECT MIX_ELM_MANAGE FROM TQMTS04 WHERE ST_NO = @st_no) ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("st_no", tqmts24.ST_NO);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts09);
		}
		else
		{
			tqmts09.HZGL = " ";
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "tqmts09.HZGL = [{0}]",(const char*)tqmts09.HZGL);

		/*------------------------  元素判定  ------------------------*/
		//对元素逐次判定
		for (i = 0; i < bcls_elm.Tables[0].Rows.get_Count(); i++ )
		{
			//1.取元素标准值
			tqmts02.MergeFrom(bcls_elm.Tables[0].Rows[i]);

			Log::Trace("", __FUNCTION__, " 第 [{0}] 项 ------ 元素代码-名称 [{1}]:[{2}]", i, (const char*)tqmts02.ELM_CODE, (const char*)tqmts02.ELM_NAME);
			Log::Trace("", __FUNCTION__, " 主试[{0}/{1}]-特采[{2}/{3}]-目标[{4}] ----",tqmts02.MAIN_MIN.ToDouble(),tqmts02.MAIN_MAX.ToDouble(),tqmts02.SPE_MIN.ToDouble(),tqmts02.SPE_MAX.ToDouble(),tqmts02.MAIN_AIM.ToDouble());

			//初始化判定结果
			elm_ok = 0;

			//modify by reason 2007-2-1 L2传C/S参考区分 无效值！！！
			//Log::Trace("", __FUNCTION__, " C/S参考区分  [{0}] ",(const char*)tqmts24.C_S_JUDGE_DIV);
			//
			//if(tqmts02.ELM_CODE=="012" || tqmts02.ELM_CODE=="032")// C|S元素
			//{
			//	if(tqmts24.C_S_JUDGE_DIV[0] == '1')//供参考,不需判定
			//	{
			//		elm_ok = 9;
			//		goto l_set_elm_ok;
			//	}
			//}
			//if(tqmts02.ELM_CODE=="001" || tqmts02.ELM_CODE=="014" || tqmts02.ELM_CODE=="016")// H|N|O元素 	    
			//{
			//	if(st_gas_sample_div[0] == '1')//以上元素不需判定
			//	{ 
			//		elm_ok = 9;
			//		goto l_set_elm_ok;
			//	}
			//} 

			//2.取成分元素实绩值
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS25 "
					" WHERE HEAT_NO = @heat_no "
					"   AND ST_SAMPLE_NO = @st_sample_no "
					"   AND ELM_CODE = @elm_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24.HEAT_NO);
			cmd_inq.Parameters.Set("st_sample_no", tqmts24.ST_SAMPLE_NO);
			cmd_inq.Parameters.Set("elm_code", tqmts02.ELM_CODE);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read()) //有实绩
			{
				cmd_inq.Fetch(tqmts25);
				Log::Trace("", __FUNCTION__, " 有实绩：实绩值 {0} ",tqmts25.ELM_ACT.ToDouble());
				if(tqmts24.ST_SAMPLE_DIV=="1")//QV样中, 对'H,N,O'元素不做判定
				{ 
					Log::Trace("", __FUNCTION__, " QV样 ");
					if(tqmts02.ELM_CODE=="001" || tqmts02.ELM_CODE=="014" || tqmts02.ELM_CODE=="016")// H|N|O元素 	  
					{
						elm_ok = 9;/*9-不需要判定*/
					}
				}
				else if(tqmts24.ST_SAMPLE_DIV=="4") //气体样中, 对非'H,N,O'元素不做判定
				{
					Log::Trace("", __FUNCTION__, " 气体样 ");
					//ON样、H样：考虑气体区分指示，来决定该成分是否需要判定。
					if(tqmts02.ELM_CODE!="001" && tqmts02.ELM_CODE!="014" && tqmts02.ELM_CODE!="016")// H|N|O元素 	  
					{
						elm_ok = 9;
					}
					switch(CDecimal::Parse(tqmts24.GAS_TYPE_DIV).ToInt16())
					{
						//GAS_TYPE_DIV:1:ON样 2:H样
					case 1: //进行ON样判定
						Log::Trace("", __FUNCTION__, " ON样 ");
						if(tqmts02.ELM_CODE=="014" && tqmts0x.N_GAS_DIV!="1")
						{
							elm_ok = 9;
						}
						else if(tqmts02.ELM_CODE=="016" && tqmts0x.O_GAS_DIV!="1")
						{
							elm_ok = 9;
						}
						break;
					case 2: //进行H样判定
						Log::Trace("", __FUNCTION__, " H样 ");
						if(tqmts02.ELM_CODE=="001" && tqmts0x.H_GAS_DIV!="1")
						{
							elm_ok = 9;
						}
						break;
					default:
						break;
					}
				}
				//未有判定结果的(即无特殊要求，走正常判定流程的)
				if (elm_ok == 0)  // 0:未判定
				{
					//add by reason 2007-3-13
					//要求判定
					//1.标准值为0～0则，判混杂元素  混杂元素管理区分在制造标准中:有值,则判; 无值，判不合。
					//2.工序成分的判定, 除了连铸工序以外, 都只判主试
					if (tqmts02.MAIN_MIN == 0 && tqmts02.MAIN_MAX == 0 && tqmts0x.SMELT_DIV == "B") //标准值为(0,0),则判混杂元素
					{
						Log::Trace("", __FUNCTION__, "混杂元素区分[{0}]",(const char*)tqmts09.HZGL);
						if (tqmts09.HZGL == " ") //管理区分无, 则判该元素不合, 用以提示用户该出钢记号没有指定混杂判定号
						{
							//没必要去判成分数据，标准值为(0,0)肯定不合
							elm_ok = 2;
						}
						else
						{
							Log::Trace("", __FUNCTION__, "判混杂元素 tqmts02.ELM_CODE=[{0}]",(const char*)tqmts02.ELM_CODE);
							//该逻辑有问题，后续修改
							switch (CDecimal::Parse(tqmts02.ELM_CODE).ToInt16())
								//switch ( 1 )
							{
							case 64: //铜,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.CU_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								cu_ni_cr = cu_ni_cr + tqmts25.ELM_ACT;
								break;
							case 52: //铬,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.CR_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								cu_ni_cr = cu_ni_cr + tqmts25.ELM_ACT;
								break;
							case 11: //硼,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.B_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							case 75: //砷,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.AS_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							case 96: //钼,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.MO_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								break;
							case 93: //铌,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.NB_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							case 58: //镍,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.NI_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								cu_ni_cr = cu_ni_cr + tqmts25.ELM_ACT;
								break;
							case 119: //锡,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.SN_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							case 48: //钛,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.TI_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							case 51: //钒,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.V_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							case 207: //铅,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.PB_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								break;
							case 65: //锌,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.ZN_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								break;
							case 209: //铋,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.BI_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								break;
							case 122: //锑,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.SB_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								break;
							case 91: //锆,不判标准,判混杂元素
								if(tqmts25.ELM_ACT <= tqmts09.ZR_MAX)
								{
									elm_ok = 8;  //合格
								}
								else
								{
									elm_ok = 2;  //不合格
								}
								as_sn_nb_v_ti_zr_b = as_sn_nb_v_ti_zr_b + tqmts25.ELM_ACT;
								break;
							default:
								break;
							}
						}//管理区分有,判混杂
					}
					else   //不判混杂元素 ,按工序成分标准判定
					{
						if(tqmts24.WHOLE_BACKLOG_CODE != "C" && tqmts24.WHOLE_BACKLOG_CODE != "I")
						{
							Log::Trace("", __FUNCTION__, "工序[{0}],主试标准[{1}]-[{2}],实绩[{3}]",(const char*)tqmts24.WHOLE_BACKLOG_CODE,tqmts02.MAIN_MIN.ToDouble(),tqmts02.MAIN_MAX.ToDouble(),tqmts25.ELM_ACT.ToDouble());
							//除了连铸模铸工序以外，都只判主试
							if(tqmts25.ELM_ACT >= tqmts02.MAIN_MIN && tqmts25.ELM_ACT <= tqmts02.MAIN_MAX)//主试合
							{
								elm_ok = 8;
							}
							else //主试不合且无特采--?????用户约定
							{
								elm_ok = 2;
								if(tqmts02.SPE_MAX >= 99.999)//主试不合且无特采要求
								{
									elm_ok = 2;
								}
								else
								{
									if(tqmts25.ELM_ACT >= tqmts02.SPE_MIN &&
										(tqmts25.ELM_ACT <= tqmts02.SPE_MAX ||(fabs(tqmts02.SPE_MAX.ToDouble() - tqmts25.ELM_ACT.ToDouble()) <= 0.000009)))//主试不合但特采合
									{
										//3:主试不合但特采合
										elm_ok = 3;
									}
									else  //主试不合且特采不合
									{
										elm_ok = 1;
									}
								}//特采要求
							}
						}
						else //连铸工序，跟非连铸工序的差别是，判主试合格多了个“或”条件：实绩值和最大值之差的绝对值不超过0.000009
						{
							Log::Trace("", __FUNCTION__, "连铸工序,主试标准[{0}-{1}],实绩[{2}]",tqmts02.MAIN_MIN.ToDouble(),tqmts02.MAIN_MAX.ToDouble(),tqmts25.ELM_ACT.ToDouble());
							if(tqmts25.ELM_ACT >= tqmts02.MAIN_MIN &&
								(tqmts25.ELM_ACT < tqmts02.MAIN_MAX ||(fabs(tqmts02.MAIN_MAX.ToDouble() - tqmts25.ELM_ACT.ToDouble()) <= 0.000009)))//主试合
							{
								elm_ok = 8;
							}
							else //主试不合
							{
								if(tqmts02.SPE_MAX >= 99.999)//主试不合且无特采要求
								{
									elm_ok = 2;
								}
								else
								{
									if(tqmts25.ELM_ACT >= tqmts02.SPE_MIN &&
										(tqmts25.ELM_ACT <= tqmts02.SPE_MAX ||(fabs(tqmts02.SPE_MAX.ToDouble() - tqmts25.ELM_ACT.ToDouble()) <= 0.000009)))//主试不合但特采合
									{									
										elm_ok = 3;
									}
									else  //主试不合且特采不合
									{
										elm_ok = 1;
									}
								}//特采要求
							}//主试判断
						}//连铸工序
					}////不判混杂元素 ,按工序成分标准判定 end
				}// 0:未判定的元素进行判定
				Log::Trace("", __FUNCTION__, " 判定结果 = [{0}] ",elm_ok);
			}//有实绩
			else //没有实绩
			{
				Log::Trace("", __FUNCTION__, " 没有实绩 ");
				if(tqmts24.ST_SAMPLE_DIV=="1")//QV样中, 对'H,N,O'元素不做判定
				{ 
					Log::Trace("", __FUNCTION__, " QV样 ");
					if(tqmts02.ELM_CODE=="001" || tqmts02.ELM_CODE=="014" || tqmts02.ELM_CODE=="016")// H|N|O元素 	  
					{
						elm_ok = 9;
					}
				}
				else if(tqmts24.ST_SAMPLE_DIV=="4") //气体样中, 对非'H,N,O'元素不做判定
				{
					Log::Trace("", __FUNCTION__, " 气体样 ");
					//ON样、H样：考虑气体区分指示，来决定该成分是否需要判定。
					if(tqmts02.ELM_CODE!="001" && tqmts02.ELM_CODE!="014" && tqmts02.ELM_CODE!="016")// H|N|O元素 	  
					{
						elm_ok = 9;
					}
					switch(CDecimal::Parse(tqmts24.GAS_TYPE_DIV).ToInt16())
					{
						//GAS_TYPE_DIV:1:ON样 2:H样
					case 1: //进行ON样判定
						Log::Trace("", __FUNCTION__, " ON样 ");
						if(tqmts02.ELM_CODE=="014" && tqmts0x.N_GAS_DIV!="1")
						{
							elm_ok = 9;
						}
						else if(tqmts02.ELM_CODE=="016" && tqmts0x.O_GAS_DIV!="1")
						{
							elm_ok = 9;
						}
						break;
					case 2: //进行H样判定
						Log::Trace("", __FUNCTION__, " H样 ");
						if(tqmts02.ELM_CODE=="001" && tqmts0x.H_GAS_DIV!="1")
						{
							elm_ok = 9;
						}
						break;
					default:
						break;
					}
				}
				//未有判定结果的(即无特殊要求，走正常判定流程的)
				if (elm_ok == 0)  // 0:未判定
				{
					if(tqmts02.MAIN_MIN == 0 && tqmts02.MAIN_MAX >= 99.999)
						elm_ok = 8;//标准无要求，没有实绩判成合格
					else
						elm_ok = 1;//标准有要求，没有实绩判成不合格
				}
				Log::Trace("", __FUNCTION__, " 判定结果 = [{0}] ",elm_ok);
			}//没有实绩
			cmd_inq.Close();

			//更新工序成分表的成分合否标志
			tqmts25.ELM_OK = elm_ok;
			tqmts25.HEAT_NO = tqmts24.HEAT_NO;
			tqmts25.ST_SAMPLE_NO = tqmts24.ST_SAMPLE_NO;
			tqmts25.ELM_CODE = tqmts02.ELM_CODE;

			if(tqmts25.ELM_OK == 0 )
				elm_0_num ++;
			else if(tqmts25.ELM_OK == 1 || tqmts25.ELM_OK == 2 || tqmts25.ELM_OK == 3)
				elm_1_num ++;

			sqlstr = CString("tqmts25.Update(\"ELM_OK\", \"HEAT_NO, ST_SAMPLE_NO, ELM_CODE\" ");
			tqmts25.Update("ELM_OK",  //修改字段项
				"HEAT_NO, ST_SAMPLE_NO, ELM_CODE"); //条件字段项
		}// for i

		/*------------------------  炼钢试样判定  ------------------------*/
		Log::Trace("", __FUNCTION__, " elm_0_num /elm_1_num  [{0}]:[{1}] ",elm_0_num, elm_1_num);	
		if(elm_0_num > 0)
		{
			tqmts24.JUDGE_CODE = "0";//未判定
		}
		else if(elm_1_num > 0)
		{
			tqmts24.JUDGE_CODE = "2";//不合格
		}
		else  
		{
			tqmts24.JUDGE_CODE = "1";//合格
		}	

		//add by reason 2007-3-13
		//判定混杂元素，组合不合格把每个元素都置成不合
		if(cu_ni_cr > tqmts09.CU_NI_CR && tqmts09.HZGL.TrimOrBlank() != " ")
		{
			tqmts24.JUDGE_CODE = "2";//不合格
			cmd_upd.SetCommandText(" UPDATE TQMTS25 SET elm_ok = 2 WHERE HEAT_NO = @heat_no AND ST_SAMPLE_NO = @st_sample_no AND ELM_CODE IN ('052','058','064') ");
			cmd_upd.Parameters.Set("heat_no", tqmts24.HEAT_NO);
			cmd_upd.Parameters.Set("st_sample_no", tqmts24.ST_SAMPLE_NO);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}

		if(as_sn_nb_v_ti_zr_b > tqmts09.AS_SN_NB_V_TI_ZR_B && tqmts09.HZGL.TrimOrBlank() != " ")
		{
			tqmts24.JUDGE_CODE = "2";//不合格
			cmd_upd.SetCommandText(" UPDATE TQMTS25 SET ELM_OK = 2 WHERE HEAT_NO = @heat_no AND ST_SAMPLE_NO = @st_sample_no AND ELM_CODE IN ('011','048','051','075','093','119') ");
			cmd_upd.Parameters.Set("heat_no", tqmts24.HEAT_NO);
			cmd_upd.Parameters.Set("st_sample_no", tqmts24.ST_SAMPLE_NO);
			cmd_upd.ExecuteNonQuery();
			cmd_upd.Close();
		}

		/*------------------------  更新 没有判定标准的元素,置其成分合否标志为9  ------------------------*/
		cmd_upd.SetCommandText(" UPDATE TQMTS25 SET ELM_OK = 9 WHERE HEAT_NO = @heat_no AND ST_SAMPLE_NO = @st_sample_no AND ELM_OK = 0 ");
		cmd_upd.Parameters.Set("heat_no", tqmts24.HEAT_NO);
		cmd_upd.Parameters.Set("st_sample_no", tqmts24.ST_SAMPLE_NO);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		/*---------------------  更新炼钢试样判定结果  ---------------------*/
		sqlstr = CString("tqmts24.Update(\"JUDGE_CODE\", \"HEAT_NO, ST_SAMPLE_NO\" ");
		tqmts24.Update("JUDGE_CODE",  //修改字段项
			"HEAT_NO, ST_SAMPLE_NO"); //条件字段项
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
		strncpy(s.sysmsg, "System Exception", sizeof(s.sysmsg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

