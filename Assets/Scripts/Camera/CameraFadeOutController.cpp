import json
from pathlib import Path

PROTOTYPE_SCENE = "PrototypeMap.scene"
REFERENCE_SCENE = "MapParactice.scene"
OUTPUT_SCENE = "PrototypeMap_Roughness1.scene"


def is_tile_entity(entity: dict) -> bool:
sm = entity.get("SkinnedMesh")
if not isinstance(sm, dict) :
	return False

	inst = str(sm.get("instanceAssetPath", "")).lower()
	mesh = str(sm.get("meshAssetPath", "")).lower()

	return ("tile0205" in inst) or (mesh == "tile0205")


	def extract_reference_tile_material(ref_scene: dict)->dict:
"""
MapParactice.scene 에서 타일 하나의 Material을 찾아서 복사
"""
for e in ref_scene.get("entities", []) :
	if is_tile_entity(e) and isinstance(e.get("Material"), dict) :
		return e["Material"]
		raise RuntimeError("MapParactice.scene에서 타일 Material을 찾지 못했습니다.")


		def main() :
		# 파일 로드
		proto = json.loads(Path(PROTOTYPE_SCENE).read_text(encoding = "utf-8"))
		ref = json.loads(Path(REFERENCE_SCENE).read_text(encoding = "utf-8"))

		# 기준 Material 추출
		reference_material = extract_reference_tile_material(ref)

		changed = 0

		for e in proto.get("entities", []) :
			if not is_tile_entity(e) :
				continue

				# Material 깊은 복사
				new_mat = json.loads(json.dumps(reference_material))

				# ✅ roughness 강제 1
				new_mat["roughness"] = 1.0

				e["Material"] = new_mat
				changed += 1

				# 저장
				Path(OUTPUT_SCENE).write_text(
					json.dumps(proto, indent = 4, ensure_ascii = False),
					encoding = "utf-8"
				)

				print("변환 완료")
				print(f"- 타일 Material 적용 개수: {changed}")
				print(f"- 출력 파일: {OUTPUT_SCENE}")


				if __name__ == "__main__":
main()
