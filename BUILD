exports_files(["debug-tokenized.log"])

cc_binary(
  name = "main",
  deps = [
		"@com_google_absl//absl/strings",
		"@com_google_absl//absl/container:flat_hash_map",
		"@com_google_absl//absl/types:span",
	],
  srcs = ["main.cc"],
	data = [
		":debug-tokenized.log",
	]
)

