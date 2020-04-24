#include "json.h"

json_error_t *jsonerror;

void jansson_init(void)
{
	json_set_alloc_funcs(user_malloc, user_free);
}

//创建温度曲线json文件
int CreateTemp_Jsonfile(const char *path)
{
	json_t *jdata,*jsubdata;
	json_t *jarr,*jarr1,*jarr2,*jval;
	char *out;
	u8 i,j;
	int ret;

	jdata = json_object();
	jarr = json_array();
	jsubdata = json_pack_ex(jsonerror,0,"{sbsi}", "Enable", temp_data.HeatCoverEnable,"Temp",temp_data.HeatCoverTemp);
	json_object_set(jdata,"HeartCover",jsubdata);
	jval = json_integer(temp_data.StageNum);
	json_array_append_new( jarr, jval );

	for(i=0;i<temp_data.StageNum;i++)	{
		jarr2 = json_array();
		jsubdata = json_pack_ex(jsonerror,0,"{si}", "Type",temp_data.stage[i].Type);
		for(j=0;j<temp_data.stage[i].StepNum;j++)	{
			jarr1 = json_array();
			jval = json_integer( temp_data.stage[i].step[j].temp );
			json_array_append_new( jarr1, jval );
			jval = json_integer( temp_data.stage[i].step[j].tim );
			json_array_append_new( jarr1, jval );
			jval = json_integer( temp_data.stage[i].step[j].CollEnable );
			json_array_append_new( jarr1, jval );
			json_array_append_new( jarr2, jarr1 );
		}
		json_object_set_new(jsubdata, "Step", jarr2);
		if(temp_data.stage[i].Type==StageByRepeat)
			json_object_set_new(jsubdata, "Repeat", json_integer(temp_data.stage[i].RepeatNum));
		else if(temp_data.stage[i].Type==StageByContinue)//溶解曲线
			json_object_set_new(jsubdata, "T_Rate", json_integer(temp_data.stage[i].T_Rate));
		else if(temp_data.stage[i].Type==StageByContinue)	{//溶解曲线
			json_object_set_new(jsubdata, "T_Inter", json_integer(temp_data.stage[i].T_Inter));
			json_object_set_new(jsubdata, "Const_Tim", json_integer(temp_data.stage[i].Const_Tim));
		}
		json_array_append_new( jarr, jsubdata );
	}
	json_object_set(jdata,"Stage",jarr);
		
//	out = json_dumps(jdata, JSON_INDENT(1));
//	SYS_PRINTF("out:%s", out);
	
	ret = json_dump_file(jdata, path, JSON_INDENT(1)|JSON_PRESERVE_ORDER);
	json_delete(jdata);
	json_delete(jsubdata);
//	user_free(out);
	return ret;
}

//创建温度曲线json文件
void CreateLab_Jsonfile(void)
{
	json_t *jdata,*jsubdata;
	json_t *jarr;
	char *out;
	u8 i;
	
	jdata = json_object();
	jarr = json_array();
	
	jsubdata = json_pack_ex(jsonerror,0,"{ss,ss,si,si,si}", "id", lab_data.id,"name",lab_data.name,"type",lab_data.type,"method",lab_data.method,"HoleNum",HOLE_NUM);
	json_object_set(jdata,"Lab",jsubdata);
	for(i=0;i<HOLE_NUM;i++)	{
		if(sample_data.hole[i].sample_t != 0)	
		{
//			jsubdata = json_pack_ex(jsonerror,0,"{si,ss,ss,ss,ss}", "ID", i+1,"name",sample_data.hole[i].name,"prj",sample_data.hole[i].prj,\
//						"sample",&sample_data.hole[i].sample_t,"ch",sample_data.hole[i].channel);
			jsubdata = json_pack("{si,ss,ss}", "ID", i+1, "name",sample_data.hole[i].name, "prj",sample_data.hole[i].prj);	
			json_object_set_new(jsubdata, "sample", json_stringn((const char *)&sample_data.hole[i].sample_t,1));
			json_object_set_new(jsubdata, "ch", json_string(sample_data.hole[i].channel));
			json_array_append_new( jarr, jsubdata );
		}
	}
	json_object_set(jdata,"Hole",jarr);
	
	out = json_dumps(jdata, JSON_INDENT(1)|JSON_PRESERVE_ORDER);
	SYS_PRINTF("out:%s", out);
	json_delete(jdata);
	json_delete(jsubdata);
	user_free(out);
}

s8 AnalysisTemp_Jsonfile(const char *path)
{
	json_t *jdata,*jsubdata;

	jdata = json_load_file(path, JSON_DECODE_ANY, jsonerror);
	jsubdata = json_object_get(jdata,"HeatCover");
	if(jsubdata!=NULL)	{
		if(json_is_object(jsubdata))	{
			json_t* tmp = json_object_get(jsubdata,"Enable");
			temp_data.HeatCoverEnable = json_real_value(tmp);
			json_decref(tmp);
			tmp = json_object_get(jsubdata,"Temp");
			temp_data.HeatCoverTemp = json_real_value(tmp);
			json_decref(tmp);
		}
	}
}


//添加二维数组到json文件
void add_2array_to_json( json_t* obj, const char* name, const int* arr_adrr, size_t row, size_t column)
{
	size_t i, j;
	json_t *jarr1,*jarr2;
	json_t* jval;
	int val;
	
	jarr1 = json_array();
	for( i=0; i<row; ++i ) {
		jarr2 = json_array();
		for( j=0; j<column; ++j ) {
			val = arr_adrr[ i*column + j ];
			jval = json_integer( val );
			json_array_append_new( jarr2, jval );
		}
		json_array_append_new( jarr1, jarr2 );
	}
	json_object_set_new( obj, name, jarr1 );
}












//jansson Test
//int arr1[2][3] = { {1,2,3}, {4,5,6} };
//int arr2[4][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
void jansson_pack_test(void)
{
	json_t *root;
	char *out;
	
	/* Build an empty JSON object */
	root = json_pack("{}");
	
	out = json_dumps(root, JSON_ENCODE_ANY);
	SYS_PRINTF("out:%s", out);
	json_delete(root);
	user_free(out);
	
	/* Build the JSON object {"foo": 42, "bar": 7} */
//	root = json_pack("{sisi}", "foo", 42, "bar", 7);
	root = json_pack_ex(jsonerror,0,"{sisi}", "foo", 42, "bar", 5);

	out = json_dumps(root, JSON_ENCODE_ANY);
	SYS_PRINTF("out:%s", out);
	json_delete(root);
	user_free(out);
	
	/* Like above, ':', ',' and whitespace are ignored */
	root = json_pack("{s:i, s:i}", "foo", 42, "bar", 7);

	out = json_dumps(root, JSON_ENCODE_ANY);
	SYS_PRINTF("out:%s", out);
	json_delete(root);
	user_free(out);
	
	/* Build the JSON array [[1, 2], {"cool": true}] */
	root = json_pack("[[i,i],{s:b}]", 1, 2, "cool", 1);

	out = json_dumps(root, JSON_ENCODE_ANY);
	SYS_PRINTF("out:%s", out);
	json_delete(root);
	user_free(out);
	
	/* Build a string from a non-null terminated buffer */
	char buffer[4] = {'t', 'e', 's', 't'};
	root = json_pack("[s#]", buffer, 4);

	out = json_dumps(root, JSON_ENCODE_ANY);
	SYS_PRINTF("out:%s", out);
	json_delete(root);
	user_free(out);
	
	/* Concatenate strings together to build the JSON string "foobarbaz" */
	root = json_pack("[s++]", "foo", "bar", "baz");
	
	out = json_dumps(root, JSON_ENCODE_ANY);
	SYS_PRINTF("out:%s", out);
	json_delete(root);
	user_free(out);
//out:{"arr1": [[1, 2, 3], [4, 5, 6]], "arr2": [[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]]}
//	root = json_object();
//	add_2array_to_json( root, "arr1", &arr1[0][0], 2, 3 );
//	add_2array_to_json( root, "arr2", &arr2[0][0], 4, 4 );
//	out = json_dumps( root, JSON_ENCODE_ANY );
//	SYS_PRINTF("out:%s", out);
//	json_delete(root);
//	user_free(out);
}

