// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012-2014, 2017-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022-2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/sysfs.h>
#include <linux/remoteproc.h>
#include <linux/slab.h>

#define BOOT_CMD 1
#define IMAGE_UNLOAD_CMD 0

static ssize_t cdsp_boot_store(struct kobject *kobj,
	struct kobj_attribute *attr,
	const char *buf, size_t count);

struct cdsp_loader_private {
	struct rproc *pil_h;
	struct mutex lock;
	bool booted;
	struct kobject *boot_cdsp_obj;
};

static struct kobj_attribute cdsp_boot_attribute =
	__ATTR(boot, 0220, NULL, cdsp_boot_store);

static struct attribute *attrs[] = {
	&cdsp_boot_attribute.attr,
	NULL,
};

static const struct attribute_group cdsp_attr_group = {
	.attrs = attrs,
};

static struct cdsp_loader_private *cdsp_private;

static ssize_t cdsp_boot_store(struct kobject *kobj,
	struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct cdsp_loader_private *priv = cdsp_private;
	u32 boot;
	int ret;

	ret = kstrtou32(buf, 0, &boot);
	if (ret)
		return ret;
	if (boot != BOOT_CMD && boot != IMAGE_UNLOAD_CMD)
		return -EINVAL;
	if (!priv)
		return -ENODEV;

	mutex_lock(&priv->lock);
	ret = 0;
	if (boot == BOOT_CMD && !priv->booted) {
		ret = rproc_boot(priv->pil_h);
		if (!ret)
			priv->booted = true;
	} else if (boot == IMAGE_UNLOAD_CMD && priv->booted) {
		ret = rproc_shutdown(priv->pil_h);
		if (!ret)
			priv->booted = false;
	}
	mutex_unlock(&priv->lock);

	if (ret)
		return ret;
	return count;
}

static int cdsp_loader_remove(struct platform_device *pdev)
{
	struct cdsp_loader_private *priv = platform_get_drvdata(pdev);
	int ret;

	/* Drain boot/unload stores before releasing their state and rproc. */
	sysfs_remove_group(priv->boot_cdsp_obj, &cdsp_attr_group);
	cdsp_private = NULL;
	if (priv->booted) {
		ret = rproc_shutdown(priv->pil_h);
		if (ret)
			dev_err(&pdev->dev, "CDSP shutdown failed: %d\n", ret);
	}
	rproc_put(priv->pil_h);
	kobject_put(priv->boot_cdsp_obj);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static int cdsp_loader_probe(struct platform_device *pdev)
{
	struct cdsp_loader_private *priv;
	struct rproc *cdsp;
	const char *img_name;
	u32 rproc_phandle;
	int ret;

	ret = of_property_read_string(pdev->dev.of_node, "qcom,proc-img-to-load",
				      &img_name);
	if (ret)
		return ret;
	if (strcmp(img_name, "cdsp"))
		return -EINVAL;

	ret = of_property_read_u32(pdev->dev.of_node, "qcom,rproc-handle",
				   &rproc_phandle);
	if (ret)
		return ret;
	cdsp = rproc_get_by_phandle(rproc_phandle);
	if (!cdsp)
		return -EPROBE_DEFER;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		ret = -ENOMEM;
		goto err_put_rproc;
	}
	priv->pil_h = cdsp;
	mutex_init(&priv->lock);
	platform_set_drvdata(pdev, priv);

	priv->boot_cdsp_obj = kobject_create_and_add("boot_cdsp", kernel_kobj);
	if (!priv->boot_cdsp_obj) {
		ret = -ENOMEM;
		goto err_put_rproc;
	}

	/* Publish fully initialized state before the boot attribute is visible. */
	cdsp_private = priv;
	ret = sysfs_create_group(priv->boot_cdsp_obj, &cdsp_attr_group);
	if (ret) {
		cdsp_private = NULL;
		kobject_put(priv->boot_cdsp_obj);
		goto err_put_rproc;
	}
	return 0;

err_put_rproc:
	platform_set_drvdata(pdev, NULL);
	rproc_put(cdsp);
	return ret;
}

static const struct of_device_id cdsp_loader_dt_match[] = {
	{ .compatible = "qcom,cdsp-loader" },
	{ }
};
MODULE_DEVICE_TABLE(of, cdsp_loader_dt_match);

static struct platform_driver cdsp_loader_driver = {
	.driver = {
		.name = "cdsp-loader",
		.of_match_table = cdsp_loader_dt_match,
	},
	.probe = cdsp_loader_probe,
	.remove = cdsp_loader_remove,
};

static int __init cdsp_loader_init(void)
{
	return platform_driver_register(&cdsp_loader_driver);
}
module_init(cdsp_loader_init);

static void __exit cdsp_loader_exit(void)
{
	platform_driver_unregister(&cdsp_loader_driver);
}
module_exit(cdsp_loader_exit);

MODULE_DESCRIPTION("CDSP Loader module");
MODULE_LICENSE("GPL v2");
