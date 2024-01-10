#include "pch.h"
#include "CaretTipOptions.h"

void to_json(nlohmann::json& j, const CaretTipOptions& p)
{
	j = nlohmann::json{ {"show", p.ShowTip}, {"textColor", p.TextColor}, {"bkColor", p.BkColor} };
}

void from_json(const nlohmann::json& j, CaretTipOptions& p)
{
	j.at("show").get_to(p.ShowTip);
	j.at("textColor").get_to(p.TextColor);
	j.at("bkColor").get_to(p.BkColor);
}
