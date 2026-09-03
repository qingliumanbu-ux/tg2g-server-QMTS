/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      wxh
Version:     1.0
Date:        2016-09-21
Description: 成分预测函数   
**************************************************/
//来自柳钢，目前没有哪里调用本函数！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

/*************************************************
根据相同钢种的前5炉每炉最后一个B样或J样与对应炉次C样的偏差值的平均值作为修正值，对当前炉次最后一个B样或J样进行修正得到该炉次的预测样Y样，然后根据Y样进行判定。有J样则必须用J样。前5炉为一周时间内数据，当一周时间内数据不足5炉时，Y样预测成分全置0。
如：本炉次为123456，前5炉相同钢种分别为123441、123446、123449、123457、123459，其对应的最有一个样分别为J2、B1、J2、B2、J3、B2。
则123456炉次Y样每个元素计算公式为：
123456Y = 123456B2+（（23441C1-123441B1）+（123446C1-123446J2）+（123449C1-123449B2）+（123457C1-123457J3）+（123459C1-123459B2））/5
如果Y样的成分符合内控成分要求则允许辊道直送，否则的话只能下线入库，待C样的成分实绩进行判定后进行后续的转库或改判。
**************************************************/


/*************************************************
1、获取传入参数heat_no并校验。
2、查询该炉是否有连铸样，有则提示“已有连铸样”并结束程序；无连铸样则查询是否有B样或J样，若无则报错“无法计算”并结束程序。
3、查询一周内是否有该炉同钢种的5炉连铸样，无则报错并结束程序。
4、按钢种的成分内控标准循环，获取5炉的连铸样，获取5炉的B样或J样（先J后B，若都无则取C样、认为无偏差？），进行计算并按内控进行判定，若不合则记入提示。
**************************************************/

/*************************************************
输出块
列名：heat_no、试样号、元素C、Si、Mn......
行：共
第一、二行标准上下限
第三行预测出的连铸样
第四行合格标记
第五行传入炉的B、J样
第六到十五行5炉的C和BJ样

|行号|rows|heat_no|sample_no|  C    |   Si   |   Mn...
|  1   |   0    |              |                    | min | min |  min
|  2   |   1    |              |                    | max| max |  max
|  3   |   2    |   传入   |         Y         |   ..    |   ..    |    ..
|  4   |   3    |              |                    |   合  | 不合 |     
|  5   |   4    |   传入   |       B、J      |   ..    |   ..    |    ..
|  6   |   5    |   炉一   |         C         |   ..    |   ..    |    ..
|  7   |   6    |   炉一   |       B、J      |   ..    |   ..    |    ..
|  8   |   7    |   炉二   |         C         |   ..    |   ..    |    ..
|  9   |   8    |   炉二   |       B、J      |   ..    |   ..    |    ..
|  10 |   9    |   炉三   |         C         |   ..    |   ..    |    ..
|  11 |   10  |   炉三   |       B、J      |   ..    |   ..    |    ..
|  12 |   11  |   炉四   |         C         |   ..    |   ..    |    ..
|  13 |   12  |   炉四   |       B、J      |   ..    |   ..    |    ..
|  14 |   13  |   炉五   |         C         |   ..    |   ..    |    ..
|  15 |   14  |   炉五   |       B、J      |   ..    |   ..    |    ..
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中



/*  外部函数申明  */


BM2_FUNCTION_EXPORT
 int f_qmts_inqy(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CDecimal elm_act = 0;
	int elm_ok = 0;
	CDecimal Count = 0;

	CString		s_heat_no						= ""	;
	CString		s_st_sample_no			= ""	;

	CString		sqlstr							= ""	;

	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tqmts25("TQMTS25");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq24(conn);

	//输出块
	if (!bcls_ret->Tables[0].Columns.Contains("HEAT_NO"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	}
	if (!bcls_ret->Tables[0].Columns.Contains("ST_SAMPLE_NO"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	}
	if (!bcls_ret->Tables[0].Columns.Contains("WHOLE_BACKLOG_CODE"))
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
	}
	for (i = 0; i < 15; i++)
	{
		bcls_ret->Tables[0].Rows.Add();
	}
	//第三行预测出的连铸样、第五行传入炉的B、J样
	bcls_ret->Tables[0].Rows[2]["HEAT_NO"] = s_heat_no;
	bcls_ret->Tables[0].Rows[4]["HEAT_NO"] = s_heat_no;

	try
	{
		Log::Trace("", "", "1、获取传入参数heat_no并校验。");
		/*************************************************
		1、获取传入参数heat_no并校验。
		**************************************************/
		/*获得传入参数*/
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			s_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank();
		}

		Log::Trace("",__FUNCTION__,"f_qmts_inqy IN:---s_heat_no			[{0}]",s_heat_no);

		//校验传入参数
		if(" " == s_heat_no)
		{
			Log::Trace("",__FUNCTION__,"ERROR------[传入参数heat_no不允许为空]");
			strcpy(s.msg,"传入参数heat_no不允许为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", "", "2、查询该炉是否有连铸样，有则提示“已有连铸样”并结束程序；无连铸样则查询是否有B样或J样，若无则报错“无法计算”并结束程序。");
		/*************************************************
		2、查询该炉是否有连铸样，有则提示“已有连铸样”并结束程序；无连铸样则查询是否有B样或J样，若无则报错“无法计算”并结束程序。
		**************************************************/
		//查询连铸样
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count > 0)
		{
			Log::Trace("",__FUNCTION__,"预测炉次已有连铸样，无需计算。");
			strcpy(s.msg,"已有连铸样，无需计算。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//查询B样或J样
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count == 0)
		{
			Log::Trace("",__FUNCTION__,"预测炉次无B样或J样，无法计算。");
			strcpy(s.msg,"预测炉次无B样或J样，无法计算。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", "", "3、查询一周内是否有该炉同钢种的5炉连铸样，无则报错并结束程序。");
		/*************************************************
		3、查询一周内是否有该炉同钢种的5炉连铸样，无则报错并结束程序。
		**************************************************/
		//查询一周内是否有该炉同钢种的5炉连铸样
		sqlstr = " SELECT COUNT(distinct heat_no) FROM TQMTS24 "
			" WHERE ST_NO = (select st_no from tqmts23 where heat_no = @heat_no) "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			"   AND to_date(REC_CREATE_TIME, 'yyyymmddhh24miss') >= sysdate-7 "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count < 5)
		{
			Log::Trace("",__FUNCTION__,"预测炉次一周内无相同钢种5炉数据，无法计算。");
			strcpy(s.msg,"预测炉次一周内无相同钢种5炉数据，无法计算。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取5炉炉号
		fetchRowCount = 5;//输出块第六行开始
		sqlstr = " SELECT heat_no FROM TQMTS24 "
			" WHERE ST_NO = (select st_no from tqmts23 where heat_no = @heat_no) "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			"   AND to_date(REC_CREATE_TIME, 'yyyymmddhh24miss') >= sysdate-7 "
			"   group by heat_no order by MAX(REC_CREATE_TIME) desc "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			//每炉两行
			//Log::Trace("", __FUNCTION__, "fetchRowCount[{0}],HEAT_NO[{1}]。", fetchRowCount, cmd_inq.GetString(1));
			bcls_ret->Tables[0].Rows[fetchRowCount++]["HEAT_NO"] = cmd_inq.GetString(1);
			bcls_ret->Tables[0].Rows[fetchRowCount++]["HEAT_NO"] = cmd_inq.GetString(1);
			if (fetchRowCount > 14) break;//5炉后退出
		}
		cmd_inq.Close();

		Log::Trace("", "", "4、按钢种的成分内控标准循环，获取5炉的连铸样，获取5炉的B样或J样（先J后B，若都无则取C样、认为无偏差？），进行计算并按内控进行判定，若不合则记入提示。");
		/*************************************************
		4、按钢种的成分内控标准循环，获取5炉的连铸样，获取5炉的B样或J样（先J后B，若都无则取C样、认为无偏差？），进行计算并按内控进行判定，若不合则记入提示。
		**************************************************/
		//获取样号
		//获取计算炉次的B、J样（输出块第5行）
		sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq24.SetCommandText(sqlstr);
		cmd_inq24.Parameters.Set("heat_no", s_heat_no);
		cmd_inq24.ExecuteReader();
		if (cmd_inq24.Read())
		{
			bcls_ret->Tables[0].Rows[4]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
		}
		cmd_inq24.Close();
		//获取炉1的连铸样（输出块第6行）
		sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			;
		cmd_inq24.SetCommandText(sqlstr);
		cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[5]["HEAT_NO"]);
		cmd_inq24.ExecuteReader();
		if (cmd_inq24.Read())
		{
			bcls_ret->Tables[0].Rows[5]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
		}
		cmd_inq24.Close();
		//获取炉2的连铸样（输出块第8行）
		sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			;
		cmd_inq24.SetCommandText(sqlstr);
		cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[7]["HEAT_NO"]);
		cmd_inq24.ExecuteReader();
		if (cmd_inq24.Read())
		{
			bcls_ret->Tables[0].Rows[7]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
		}
		cmd_inq24.Close();
		//获取炉3的连铸样（输出块第10行）
		sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			;
		cmd_inq24.SetCommandText(sqlstr);
		cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[9]["HEAT_NO"]);
		cmd_inq24.ExecuteReader();
		if (cmd_inq24.Read())
		{
			bcls_ret->Tables[0].Rows[9]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
		}
		cmd_inq24.Close();
		//获取炉4的连铸样（输出块第12行）
		sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			;
		cmd_inq24.SetCommandText(sqlstr);
		cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[11]["HEAT_NO"]);
		cmd_inq24.ExecuteReader();
		if (cmd_inq24.Read())
		{
			bcls_ret->Tables[0].Rows[11]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
		}
		cmd_inq24.Close();
		//获取炉5的连铸样（输出块第14行）
		sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE = 'C' "
			;
		cmd_inq24.SetCommandText(sqlstr);
		cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[13]["HEAT_NO"]);
		cmd_inq24.ExecuteReader();
		if (cmd_inq24.Read())
		{
			bcls_ret->Tables[0].Rows[13]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
		}
		cmd_inq24.Close();
		//获取炉1的B、J样（输出块第7行）
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[6]["HEAT_NO"]);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count == 0)//无B、J样取C样
		{
			bcls_ret->Tables[0].Rows[6]["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[5]["ST_SAMPLE_NO"];
		}
		else
		{
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND WHOLE_BACKLOG_CODE  in ('B','J') "
				;
			cmd_inq24.SetCommandText(sqlstr);
			cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[6]["HEAT_NO"]);
			cmd_inq24.ExecuteReader();
			if (cmd_inq24.Read())
			{
				bcls_ret->Tables[0].Rows[6]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
			}
			cmd_inq24.Close();
		}
		//获取炉2的B、J样（输出块第9行）
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[8]["HEAT_NO"]);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count == 0)//无B、J样取C样
		{
			bcls_ret->Tables[0].Rows[8]["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[7]["ST_SAMPLE_NO"];
		}
		else
		{
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND WHOLE_BACKLOG_CODE  in ('B','J') "
				;
			cmd_inq24.SetCommandText(sqlstr);
			cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[8]["HEAT_NO"]);
			cmd_inq24.ExecuteReader();
			if (cmd_inq24.Read())
			{
				bcls_ret->Tables[0].Rows[8]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
			}
			cmd_inq24.Close();
		}
		//获取炉3的B、J样（输出块第11行）
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[10]["HEAT_NO"]);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count == 0)//无B、J样取C样
		{
			bcls_ret->Tables[0].Rows[10]["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[9]["ST_SAMPLE_NO"];
		}
		else
		{
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND WHOLE_BACKLOG_CODE  in ('B','J') "
				;
			cmd_inq24.SetCommandText(sqlstr);
			cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[10]["HEAT_NO"]);
			cmd_inq24.ExecuteReader();
			if (cmd_inq24.Read())
			{
				bcls_ret->Tables[0].Rows[10]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
			}
			cmd_inq24.Close();
		}
		//获取炉4的B、J样（输出块第13行）
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[12]["HEAT_NO"]);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count == 0)//无B、J样取C样
		{
			bcls_ret->Tables[0].Rows[12]["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[11]["ST_SAMPLE_NO"];
		}
		else
		{
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND WHOLE_BACKLOG_CODE  in ('B','J') "
				;
			cmd_inq24.SetCommandText(sqlstr);
			cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[12]["HEAT_NO"]);
			cmd_inq24.ExecuteReader();
			if (cmd_inq24.Read())
			{
				bcls_ret->Tables[0].Rows[12]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
			}
			cmd_inq24.Close();
		}
		//获取炉5的B、J样（输出块第15行）
		sqlstr = " SELECT COUNT(1) FROM TQMTS24 "
			" WHERE HEAT_NO = @heat_no "
			"   AND WHOLE_BACKLOG_CODE in ('B','J') "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[14]["HEAT_NO"]);
		Count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (Count == 0)//无B、J样取C样
		{
			bcls_ret->Tables[0].Rows[14]["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[13]["ST_SAMPLE_NO"];
		}
		else
		{
			sqlstr = " SELECT MAX(ST_SAMPLE_NO) FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND WHOLE_BACKLOG_CODE  in ('B','J') "
				;
			cmd_inq24.SetCommandText(sqlstr);
			cmd_inq24.Parameters.Set("heat_no", bcls_ret->Tables[0].Rows[14]["HEAT_NO"]);
			cmd_inq24.ExecuteReader();
			if (cmd_inq24.Read())
			{
				bcls_ret->Tables[0].Rows[14]["ST_SAMPLE_NO"] = cmd_inq24.GetString(1);
			}
			cmd_inq24.Close();
		}

		//按钢种的成分内控标准循环,并计算判定
		fetchRowCount = 0;
		sqlstr = " SELECT * "
			"   FROM TQMTS02 "
			"  WHERE ST_NO = (select st_no from tqmts23 where heat_no = @heat_no)  "
			"    AND SMELT_CHEMI_FLAG  = '3' "
			" AND WHOLE_BACKLOG_CODE = 'C' ORDER BY ELM_CODE ASC "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			elm_act = 0;
			fetchRowCount++;
			cmd_inq.Fetch(tqmts02);
			Log::Trace("", __FUNCTION__, " cmd_inq.Fetch(tqmts02) [{1}]:[{2}]", i, tqmts02["ELM_CODE"].ToString(), tqmts02["ELM_NAME"].ToString());
			if (!bcls_ret->Tables[0].Columns.Contains(tqmts02["ELM_NAME"].ToString()))
			{
				bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, tqmts02["ELM_NAME"].ToString());
			}
			//第一、二行标准上下限
			bcls_ret->Tables[0].Rows[0][tqmts02["ELM_NAME"].ToString()] = tqmts02["MAIN_MIN"];
			bcls_ret->Tables[0].Rows[1][tqmts02["ELM_NAME"].ToString()] = tqmts02["MAIN_MAX"];
			//获取各炉元素值
			//炉1C样
			tqmts25["HEAT_NO"] = bcls_ret->Tables[0].Rows[5]["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[5]["ST_SAMPLE_NO"];
			tqmts25["ELM_CODE"] = tqmts02["ELM_CODE"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[5][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = tqmts25["ELM_ACT"];
			//炉1B、J样
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[6]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[6][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act - tqmts25["ELM_ACT"];
			//炉2C样
			tqmts25["HEAT_NO"] = bcls_ret->Tables[0].Rows[7]["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[7]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[7][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act + tqmts25["ELM_ACT"].ToDecimal();
			//炉2B、J样
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[8]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[8][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act - tqmts25["ELM_ACT"];
			//炉3C样
			tqmts25["HEAT_NO"] = bcls_ret->Tables[0].Rows[9]["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[9]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[9][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act + tqmts25["ELM_ACT"].ToDecimal();
			//炉3B、J样
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[10]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[10][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act - tqmts25["ELM_ACT"];
			//炉4C样
			tqmts25["HEAT_NO"] = bcls_ret->Tables[0].Rows[11]["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[11]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[11][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act + tqmts25["ELM_ACT"].ToDecimal();
			//炉4B、J样
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[12]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[12][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act - tqmts25["ELM_ACT"];
			//炉5C样
			tqmts25["HEAT_NO"] = bcls_ret->Tables[0].Rows[13]["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[13]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[13][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act + tqmts25["ELM_ACT"].ToDecimal();
			//炉5B、J样
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[14]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[14][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			elm_act = elm_act - tqmts25["ELM_ACT"];
			//传入炉B、J样
			tqmts25["HEAT_NO"] = bcls_ret->Tables[0].Rows[4]["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = bcls_ret->Tables[0].Rows[4]["ST_SAMPLE_NO"];
			tqmts25.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			bcls_ret->Tables[0].Rows[4][tqmts02["ELM_NAME"].ToString()] = tqmts25["ELM_ACT"];
			//传入炉Y样
			elm_act = elm_act/5.0 + tqmts25["ELM_ACT"].ToDecimal();
			bcls_ret->Tables[0].Rows[2][tqmts02["ELM_NAME"].ToString()] = elm_act;
			//判定
			if (elm_act < tqmts02["MAIN_MIN"].ToDecimal() && elm_act > tqmts02["MAIN_MAX"].ToDecimal())
			{
				elm_ok++;
				strcat(s.msg, tqmts02["ELM_NAME"].ToString());
			}
		}
		cmd_inq.Close();

		if (elm_ok>0)
		{
			strcat(s.msg, "不合格");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", __FUNCTION__, "判定成功!");
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
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
